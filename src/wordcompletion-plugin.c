/*
 * Copyright (C) 2010 - Jeff Johnston
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <codeslayer/codeslayer.h>
#include "wordcompletion-provider.h"
#include <gtk/gtk.h>
#include <gmodule.h>
#include <glib.h>

G_MODULE_EXPORT void activate             (CodeSlayer       *codeslayer);
G_MODULE_EXPORT void deactivate           (CodeSlayer       *codeslayer);

static void          document_added_action  (CodeSlayer       *codeslayer, 
                                           CodeSlayerDocument *document);
                                                                                      
gulong document_added_id;

G_MODULE_EXPORT void
activate (CodeSlayer *codeslayer)
{
  GList *documents;
  GList *tmp;

  documents = codeslayer_get_all_documents (codeslayer);
  
  tmp = documents;
  
  while (tmp != NULL)
    {
      CodeSlayerDocument *document = tmp->data;
      document_added_action (codeslayer, document);
      tmp = g_list_next (tmp);
    }
    
  g_list_free (documents);
  
  document_added_id = g_signal_connect_swapped (G_OBJECT (codeslayer), "document-added",
                                              G_CALLBACK (document_added_action), NULL);
}

G_MODULE_EXPORT void 
deactivate (CodeSlayer *codeslayer)
{
  g_signal_handler_disconnect (codeslayer, document_added_id);
}

static void
document_added_action (CodeSlayer         *codeslayer, 
                       CodeSlayerDocument *document)
{
  WordCompletionProvider *provider;
  GtkSourceView *source_view;
  
  source_view = codeslayer_document_get_source_view (document);
  provider = word_completion_provider_new (document);
  
  codeslayer_source_view_add_completion_provider (CODESLAYER_SOURCE_VIEW (source_view), 
                                                  CODESLAYER_COMPLETION_PROVIDER (provider));
}

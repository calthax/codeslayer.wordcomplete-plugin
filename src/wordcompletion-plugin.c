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

static void          editor_added_action  (CodeSlayer       *codeslayer, 
                                           CodeSlayerEditor *editor);
                                                                                      
gulong editor_added_id;

G_MODULE_EXPORT void
activate (CodeSlayer *codeslayer)
{
  GList *editors;
  GList *tmp;

  editors = codeslayer_get_all_editors (codeslayer);
  
  tmp = editors;
  
  while (tmp != NULL)
    {
      CodeSlayerEditor *editor = tmp->data;
      editor_added_action (codeslayer, editor);
      tmp = g_list_next (tmp);
    }
    
  g_list_free (editors);
  
  editor_added_id = g_signal_connect_swapped (G_OBJECT (codeslayer), "editor-added",
                                              G_CALLBACK (editor_added_action), NULL);
}

G_MODULE_EXPORT void 
deactivate (CodeSlayer *codeslayer)
{
  g_signal_handler_disconnect (codeslayer, editor_added_id);
}

static void
editor_added_action (CodeSlayer       *codeslayer, 
                     CodeSlayerEditor *editor)
{
  WordCompletionProvider *provider;
  provider = word_completion_provider_new (editor);
  codeslayer_editor_add_completion_provider (editor, 
                                             CODESLAYER_COMPLETION_PROVIDER (provider));
}

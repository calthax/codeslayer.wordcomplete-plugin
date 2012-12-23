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

#include "wordcompletion-provider.h"

static void word_completion_provider_interface_init  (gpointer                     page, 
                                                      gpointer                     data);
static void word_completion_provider_class_init      (WordCompletionProviderClass *klass);
static void word_completion_provider_init            (WordCompletionProvider       *word);
static void word_completion_provider_finalize        (WordCompletionProvider       *word);

static GList* word_completion_get_proposals          (WordCompletionProvider       *word, 
                                                      GtkTextIter                   iter);
static GList* find_matches                           (gchar                        *text,
                                                      gchar                        *word);
static gint compare_match                            (gchar                        *a,
                                                      gchar                        *b);
static void move_iter_word_start                     (GtkTextIter                  *iter);
static gchar* get_text_to_search                     (GtkTextView                  *text_view, 
                                                      GtkTextIter                   iter);

#define WORD_COMPLETION_PROVIDER_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), WORD_COMPLETION_PROVIDER_TYPE, WordCompletionProviderPrivate))

typedef struct _WordCompletionProviderPrivate WordCompletionProviderPrivate;

struct _WordCompletionProviderPrivate
{
  CodeSlayerEditor *editor;
};

G_DEFINE_TYPE_EXTENDED (WordCompletionProvider,
                        word_completion_provider,
                        G_TYPE_INITIALLY_UNOWNED,
                        0,
                        G_IMPLEMENT_INTERFACE (CODESLAYER_COMPLETION_PROVIDER_TYPE,
                                               word_completion_provider_interface_init));

static void
word_completion_provider_interface_init (gpointer provider, 
                                         gpointer data)
{
  CodeSlayerCompletionProviderInterface *provider_interface = (CodeSlayerCompletionProviderInterface*) provider;
  provider_interface->get_proposals = (GList* (*) (CodeSlayerCompletionProvider *obj, GtkTextIter iter)) word_completion_get_proposals;
}

static void 
word_completion_provider_class_init (WordCompletionProviderClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = (GObjectFinalizeFunc) word_completion_provider_finalize;
  g_type_class_add_private (klass, sizeof (WordCompletionProviderPrivate));
}

static void
word_completion_provider_init (WordCompletionProvider *word)
{
}

static void
word_completion_provider_finalize (WordCompletionProvider *provider)
{
  G_OBJECT_CLASS (word_completion_provider_parent_class)->finalize (G_OBJECT (provider));
}

WordCompletionProvider*
word_completion_provider_new (CodeSlayerEditor *editor)
{
  WordCompletionProviderPrivate *priv;
  WordCompletionProvider *provider;

  provider = WORD_COMPLETION_PROVIDER (g_object_new (word_completion_provider_get_type (), NULL));
  priv = WORD_COMPLETION_PROVIDER_GET_PRIVATE (provider);
  priv->editor = editor;

  return provider;
}

static GList* 
word_completion_get_proposals (WordCompletionProvider *provider, 
                               GtkTextIter             iter)
{
  WordCompletionProviderPrivate *priv;
  GList *proposals = NULL;
  GtkTextBuffer *buffer;
  GtkTextMark *mark;
  GList *list = NULL;
  GList *tmp = NULL;
  GtkTextIter start;
  gchar *start_word;
  gchar *text;
  
  priv = WORD_COMPLETION_PROVIDER_GET_PRIVATE (provider);

  start = iter;
  move_iter_word_start (&start);
  
  start_word = gtk_text_iter_get_text (&start, &iter);
  
  if (!codeslayer_utils_has_text (start_word))
    {    
      if (start_word != NULL)
        g_free (start_word);    
      return NULL;
    }

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (priv->editor));
  mark = gtk_text_buffer_create_mark (buffer, NULL, &start, TRUE);

  text = get_text_to_search (GTK_TEXT_VIEW (priv->editor), start);
  list = find_matches (text, start_word);
  tmp = list;

  while (list != NULL)
    {
      gchar *match_text = list->data;
      CodeSlayerCompletionProposal *proposal;
      proposal = codeslayer_completion_proposal_new (match_text, g_strstrip (match_text), mark);
      proposals = g_list_prepend (proposals, proposal);
      list = g_list_next (list);
    }

  if (tmp != NULL)
    {
      g_list_foreach (tmp, (GFunc) g_free, NULL);    
      g_list_free (tmp);
    }
  
  if (start_word != NULL)
    g_free (start_word);
    
  g_free (text);

  return proposals;
}

static GList*
find_matches (gchar *text, 
              gchar *word)
{
  GList *results = NULL;
  GRegex *regex;
  gchar *concat;
  GMatchInfo *match_info;
  GError *error = NULL;
  
  concat = g_strconcat ("(\\s", word, "[a-zA-Z0-9_]+)", NULL);
  
  regex = g_regex_new (concat, 0, 0, NULL);
  
  g_regex_match_full (regex, text, -1, 0, 0, &match_info, &error);
  
  while (g_match_info_matches (match_info))
    {
      gchar *match_text = NULL;
      match_text = g_match_info_fetch (match_info, 0);
      
      if (g_list_find_custom (results, match_text, (GCompareFunc) compare_match) == NULL)
        results = g_list_prepend (results, g_strdup (match_text));
        
      g_free (match_text);
      g_match_info_next (match_info, &error);
    }
  
  g_match_info_free (match_info);
  g_regex_unref (regex);
  g_free (concat);
  
  if (error != NULL)
    {
      g_printerr ("search text for completion word error: %s\n", error->message);
      g_error_free (error);
    }
    
  return results;   
}

static gint
compare_match (gchar *a,
               gchar *b)
{
  return g_strcmp0 (a, b);
}

static void
move_iter_word_start (GtkTextIter *iter)
{
  gunichar ctext;
  gtk_text_iter_backward_char (iter);
  ctext = gtk_text_iter_get_char (iter);
  
  while (g_ascii_isalnum (ctext) || ctext == '_')
    {
      gtk_text_iter_backward_char (iter);
      ctext = gtk_text_iter_get_char (iter);
    }
    
  gtk_text_iter_forward_char (iter);    
}

static gchar*
get_text_to_search (GtkTextView *text_view, 
                               GtkTextIter  iter)
{
  GtkTextBuffer *buffer;
  GtkTextIter start;
  GtkTextIter end;
  buffer = gtk_text_view_get_buffer (text_view);      
  gtk_text_buffer_get_bounds (buffer, &start, &end);
  return gtk_text_buffer_get_text (buffer, &start, &end, FALSE);
}

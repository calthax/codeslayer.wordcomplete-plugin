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

#ifndef __WORD_COMPLETION_PROVIDER_H__
#define	__WORD_COMPLETION_PROVIDER_H__

#include <gtk/gtk.h>
#include <codeslayer/codeslayer.h>

G_BEGIN_DECLS

#define WORD_COMPLETION_PROVIDER_TYPE            (word_completion_provider_get_type ())
#define WORD_COMPLETION_PROVIDER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), WORD_COMPLETION_PROVIDER_TYPE, WordCompletionProvider))
#define WORD_COMPLETION_PROVIDER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), WORD_COMPLETION_PROVIDER_TYPE, WordCompletionProviderClass))
#define IS_WORD_COMPLETION_PROVIDER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), WORD_COMPLETION_PROVIDER_TYPE))
#define IS_WORD_COMPLETION_PROVIDER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), WORD_COMPLETION_PROVIDER_TYPE))

typedef struct _WordCompletionProvider WordCompletionProvider;
typedef struct _WordCompletionProviderClass WordCompletionProviderClass;

struct _WordCompletionProvider
{
  GInitiallyUnowned parent_instance;
};

struct _WordCompletionProviderClass
{
  GInitiallyUnownedClass parent_class;
};

GType word_completion_provider_get_type (void) G_GNUC_CONST;

WordCompletionProvider*  word_completion_provider_new  (CodeSlayerDocument *document);

G_END_DECLS

#endif /* __WORD_COMPLETION_PROVIDER_H__ */

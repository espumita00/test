/**************************************************************************/
/*  quick_open_dialog.h                                                   */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#ifndef QUICK_OPEN_DIALOG_H
#define QUICK_OPEN_DIALOG_H

#include "scene/gui/dialogs.h"

class LineEdit;
class QuickOpenResultContainer;

class QuickOpenDialog : public AcceptDialog {
	GDCLASS(QuickOpenDialog, AcceptDialog);

public:
	void popup_dialog(const Vector<StringName> &p_base_types, const Callable &p_item_selected_callback);
	QuickOpenDialog();

protected:
	virtual void cancel_pressed() override;
	virtual void ok_pressed() override;

private:
	LineEdit *search_box = nullptr;
	QuickOpenResultContainer *container = nullptr;

	Callable item_selected_callback;

	void _search_box_text_changed(const String &p_query);
};

#endif // QUICK_OPEN_DIALOG_H

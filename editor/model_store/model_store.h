/**************************************************************************/
/*  model_store.h                                                         */
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

#pragma once

#include "core/templates/vector.h"
#include "scene/main/window.h"

class Button;
class Camera3D;
class GridContainer;
class Label;
class LineEdit;
class Node3D;
class OptionButton;
class SubViewport;
class SubViewportContainer;
class VBoxContainer;

// Full-screen 3D Model Store overlay for the Sohel Engine editor.
// Browse 100 built-in procedural models in a card grid (search +
// category filter), inspect the selected model in a live 3D preview
// (drag to orbit, wheel to zoom), and insert it into the active 3D
// scene with one tap.
class ModelStoreDialog : public Window {
	GDCLASS(ModelStoreDialog, Window);

private:
	LineEdit *search_box = nullptr;
	OptionButton *category_filter = nullptr;
	GridContainer *grid = nullptr;
	Label *count_label = nullptr;

	SubViewportContainer *preview_container = nullptr;
	SubViewport *preview_viewport = nullptr;
	Camera3D *preview_camera = nullptr;
	Node3D *preview_model = nullptr;
	Label *preview_name_label = nullptr;

	Button *insert_button = nullptr;
	Label *status_label = nullptr;

	Vector<Button *> card_buttons;
	Vector<int> shown_items; // Catalog indices currently in the grid.
	int selected_item = -1;

	float preview_yaw = 0.6f;
	float preview_pitch = 0.35f;
	float preview_distance = 7.0f;
	bool preview_dragging = false;

	void _build_ui();
	void _rebuild_grid();
	void _select_item(int p_catalog_index);
	void _show_preview(int p_catalog_index);
	void _update_preview_camera();
	void _reset_preview_view();
	void _insert_selected();
	void _on_close();

	void _on_search_changed(const String &p_text);
	void _on_category_selected(int p_index);
	void _on_card_pressed(int p_grid_index);
	void _on_preview_gui_input(const Ref<InputEvent> &p_event);

protected:
	void _process(double p_delta) override;
	static void _bind_methods();

public:
	ModelStoreDialog();
};

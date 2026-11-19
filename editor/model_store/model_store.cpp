/**************************************************************************/
/*  model_store.cpp                                                       */
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

#include "model_store.h"

#include "model_store_catalog.h"

#include "core/input/input_event.h"
#include "core/math/math_funcs.h"
#include "core/object/callable_mp.h"
#include "editor/editor_interface.h"
#include "editor/undo_redo.h"
#include "scene/3d/camera_3d.h"
#include "scene/3d/directional_light_3d.h"
#include "scene/3d/node_3d.h"
#include "scene/3d/world_environment.h"
#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/grid_container.h"
#include "scene/gui/label.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/option_button.h"
#include "scene/gui/scroll_container.h"
#include "scene/gui/split_container.h"
#include "scene/gui/subviewport_container.h"
#include "scene/main/viewport.h"
#include "scene/resources/environment.h"

void ModelStoreDialog::_bind_methods() {
}

ModelStoreDialog::ModelStoreDialog() {
	set_title("3D Model Store - Sohel Engine");
	set_flag(FLAG_RESIZE_DISABLED, false);
	set_process(true);
	connect(SNAME("close_requested"), callable_mp(this, &ModelStoreDialog::_on_close));
	_build_ui();
	_rebuild_grid();
}

void ModelStoreDialog::_process(double p_delta) {
	// Slow auto-spin of the preview so models feel alive; dragging
	// takes over while the user orbits.
	if (is_visible() && !preview_dragging && preview_model != nullptr) {
		preview_yaw += (float)p_delta * 0.35f;
		_update_preview_camera();
	}
}

void ModelStoreDialog::_build_ui() {
	VBoxContainer *vb = memnew(VBoxContainer);
	vb->set_anchors_preset(Control::PRESET_FULL_RECT);
	add_child(vb);

	// Top bar: title + prominent close (X) button.
	HBoxContainer *top_bar = memnew(HBoxContainer);
	vb->add_child(top_bar);
	Label *title = memnew(Label);
	title->set_text("3D Model Store");
	title->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	top_bar->add_child(title);
	Button *close_btn = memnew(Button);
	close_btn->set_text("X");
	close_btn->set_tooltip_text("Close the Model Store");
	close_btn->set_custom_minimum_size(Size2(36, 30));
	close_btn->connect(SceneStringName(pressed), callable_mp(this, &ModelStoreDialog::_on_close));
	top_bar->add_child(close_btn);

	// Search + category filter.
	HBoxContainer *filter_bar = memnew(HBoxContainer);
	vb->add_child(filter_bar);
	search_box = memnew(LineEdit);
	search_box->set_placeholder_text("Search models...");
	search_box->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	search_box->connect(SceneStringName(text_changed), callable_mp(this, &ModelStoreDialog::_on_search_changed));
	filter_bar->add_child(search_box);
	category_filter = memnew(OptionButton);
	category_filter->add_item("All Categories", -1);
	for (int i = 0; i < ModelStoreCatalog::CAT_COUNT; i++) {
		category_filter->add_item(ModelStoreCatalog::category_name(i), i);
	}
	category_filter->connect(SceneStringName(item_selected), callable_mp(this, &ModelStoreDialog::_on_category_selected));
	filter_bar->add_child(category_filter);

	// Left: card grid. Right: live 3D preview + insert.
	HSplitContainer *split = memnew(HSplitContainer);
	split->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	vb->add_child(split);

	ScrollContainer *scroll = memnew(ScrollContainer);
	scroll->set_horizontal_scroll_mode(ScrollContainer::SCROLL_MODE_DISABLED);
	scroll->set_custom_minimum_size(Size2(420, 0));
	split->add_child(scroll);
	grid = memnew(GridContainer);
	grid->set_columns(3);
	grid->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	scroll->add_child(grid);

	VBoxContainer *preview_box = memnew(VBoxContainer);
	preview_box->set_custom_minimum_size(Size2(380, 0));
	split->add_child(preview_box);
	Label *preview_title = memnew(Label);
	preview_title->set_text("Preview - drag to orbit, scroll to zoom");
	preview_box->add_child(preview_title);

	preview_container = memnew(SubViewportContainer);
	preview_container->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	preview_container->set_stretch(true);
	preview_container->set_custom_minimum_size(Size2(380, 300));
	preview_container->connect("gui_input", callable_mp(this, &ModelStoreDialog::_on_preview_gui_input));
	preview_box->add_child(preview_container);

	preview_viewport = memnew(SubViewport);
	preview_viewport->set_size(Size2i(640, 420));
	Ref<World3D> world;
	world.instantiate();
	preview_viewport->set_world_3d(world);
	preview_container->add_child(preview_viewport);

	Ref<Environment> env;
	env.instantiate();
	env->set_background(Environment::BG_COLOR);
	env->set_bg_color(Color(0.16f, 0.17f, 0.20f));
	env->set_ambient_source(Environment::AMBIENT_SOURCE_COLOR);
	env->set_ambient_light_color(Color(1, 1, 1));
	env->set_ambient_light_energy(0.9f);
	WorldEnvironment *we = memnew(WorldEnvironment);
	we->set_environment(env);
	preview_viewport->add_child(we);

	DirectionalLight3D *sun = memnew(DirectionalLight3D);
	sun->set_rotation(Vector3(-0.9f, 0.5f, 0));
	sun->set_shadow(true);
	preview_viewport->add_child(sun);

	preview_camera = memnew(Camera3D);
	preview_camera->set_fov(50.0f);
	preview_viewport->add_child(preview_camera);
	_update_preview_camera();

	preview_name_label = memnew(Label);
	preview_name_label->set_text("Select a model to preview it.");
	preview_name_label->set_autowrap_mode(TextServer::AUTOWRAP_WORD_SMART);
	preview_box->add_child(preview_name_label);

	HBoxContainer *action_bar = memnew(HBoxContainer);
	preview_box->add_child(action_bar);
	insert_button = memnew(Button);
	insert_button->set_text("Add to Scene");
	insert_button->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	insert_button->set_disabled(true);
	insert_button->connect(SceneStringName(pressed), callable_mp(this, &ModelStoreDialog::_insert_selected));
	action_bar->add_child(insert_button);
	Button *reset_view_btn = memnew(Button);
	reset_view_btn->set_text("Reset View");
	reset_view_btn->connect(SceneStringName(pressed), callable_mp(this, &ModelStoreDialog::_reset_preview_view));
	action_bar->add_child(reset_view_btn);

	status_label = memnew(Label);
	status_label->set_autowrap_mode(TextServer::AUTOWRAP_WORD_SMART);
	preview_box->add_child(status_label);

	count_label = memnew(Label);
	vb->add_child(count_label);
}

void ModelStoreDialog::_rebuild_grid() {
	for (Button *card : card_buttons) {
		card->queue_free();
	}
	card_buttons.clear();
	shown_items.clear();

	const Vector<ModelStoreCatalog::Item> &items = ModelStoreCatalog::get_items();
	String query = search_box->get_text().strip_edges().to_lower();
	int category = category_filter->get_selected_id();

	for (int i = 0; i < items.size(); i++) {
		const ModelStoreCatalog::Item &item = items[i];
		if (category >= 0 && item.category != category) {
			continue;
		}
		if (!query.is_empty() && !item.name.to_lower().contains(query) && !item.id.contains(query)) {
			continue;
		}
		Button *card = memnew(Button);
		card->set_toggle_mode(true);
		card->set_custom_minimum_size(Size2(150, 64));
		card->set_text(item.name + "\n" + ModelStoreCatalog::category_name(item.category));
		card->set_tooltip_text(item.name + "  -  " + ModelStoreCatalog::category_name(item.category));
		card->set_pressed(i == selected_item);
		int grid_index = shown_items.size();
		card->connect(SceneStringName(pressed), callable_mp(this, &ModelStoreDialog::_on_card_pressed).bind(grid_index));
		grid->add_child(card);
		card_buttons.push_back(card);
		shown_items.push_back(i);
	}

	count_label->set_text(vformat("%d of %d models", shown_items.size(), items.size()));
}

void ModelStoreDialog::_on_search_changed(const String &p_text) {
	_rebuild_grid();
}

void ModelStoreDialog::_on_category_selected(int p_index) {
	_rebuild_grid();
}

void ModelStoreDialog::_on_card_pressed(int p_grid_index) {
	if (p_grid_index < 0 || p_grid_index >= shown_items.size()) {
		return;
	}
	_select_item(shown_items[p_grid_index]);
}

void ModelStoreDialog::_select_item(int p_catalog_index) {
	selected_item = p_catalog_index;
	for (int i = 0; i < card_buttons.size(); i++) {
		card_buttons[i]->set_pressed(shown_items[i] == selected_item);
	}
	_show_preview(p_catalog_index);
	insert_button->set_disabled(false);
}

void ModelStoreDialog::_show_preview(int p_catalog_index) {
	const Vector<ModelStoreCatalog::Item> &items = ModelStoreCatalog::get_items();
	if (p_catalog_index < 0 || p_catalog_index >= items.size()) {
		return;
	}
	const ModelStoreCatalog::Item &item = items[p_catalog_index];

	if (preview_model != nullptr) {
		preview_model->get_parent()->remove_child(preview_model);
		memdelete(preview_model);
		preview_model = nullptr;
	}
	preview_model = ModelStoreCatalog::build_model(item);
	if (preview_model != nullptr) {
		preview_viewport->add_child(preview_model);
		// Frame the model: bigger models get a farther camera.
		preview_distance = CLAMP(4.0f + item.size * 1.2f, 4.0f, 25.0f);
		preview_yaw = 0.6f;
		preview_pitch = 0.35f;
		_update_preview_camera();
	}
	preview_name_label->set_text(item.name + "  -  " + ModelStoreCatalog::category_name(item.category));
}

void ModelStoreDialog::_update_preview_camera() {
	if (preview_camera == nullptr) {
		return;
	}
	float cp = Math::cos(preview_pitch);
	Vector3 offset = Vector3(Math::sin(preview_yaw) * cp, Math::sin(preview_pitch), Math::cos(preview_yaw) * cp) * preview_distance;
	preview_camera->set_position(offset);
	preview_camera->look_at(Vector3(0, 0.8f, 0), Vector3::UP);
}

void ModelStoreDialog::_reset_preview_view() {
	preview_yaw = 0.6f;
	preview_pitch = 0.35f;
	const Vector<ModelStoreCatalog::Item> &items = ModelStoreCatalog::get_items();
	if (selected_item >= 0 && selected_item < items.size()) {
		preview_distance = CLAMP(4.0f + items[selected_item].size * 1.2f, 4.0f, 25.0f);
	} else {
		preview_distance = 7.0f;
	}
	_update_preview_camera();
}

void ModelStoreDialog::_on_preview_gui_input(const Ref<InputEvent> &p_event) {
	Ref<InputEventMouseMotion> mm = p_event;
	if (mm.is_valid() && mm->get_button_mask().has_flag(MouseButtonMask::LEFT)) {
		preview_dragging = true;
		preview_yaw -= mm->get_relative().x * 0.01f;
		preview_pitch = CLAMP(preview_pitch + mm->get_relative().y * 0.01f, -0.2f, 1.4f);
		_update_preview_camera();
		return;
	}
	Ref<InputEventMouseButton> mb = p_event;
	if (mb.is_valid()) {
		if (mb->get_button_index() == MouseButton::WHEEL_UP && mb->is_pressed()) {
			preview_distance = CLAMP(preview_distance * 0.9f, 2.5f, 25.0f);
			_update_preview_camera();
		} else if (mb->get_button_index() == MouseButton::WHEEL_DOWN && mb->is_pressed()) {
			preview_distance = CLAMP(preview_distance * 1.1f, 2.5f, 25.0f);
			_update_preview_camera();
		} else if (mb->get_button_index() == MouseButton::LEFT && !mb->is_pressed()) {
			preview_dragging = false;
		}
		return;
	}
	Ref<InputEventScreenDrag> sd = p_event;
	if (sd.is_valid()) {
		preview_dragging = true;
		preview_yaw -= sd->get_relative().x * 0.01f;
		preview_pitch = CLAMP(preview_pitch + sd->get_relative().y * 0.01f, -0.2f, 1.4f);
		_update_preview_camera();
		return;
	}
	Ref<InputEventScreenTouch> st = p_event;
	if (st.is_valid() && !st->is_pressed()) {
		preview_dragging = false;
	}
}

void ModelStoreDialog::_insert_selected() {
	const Vector<ModelStoreCatalog::Item> &items = ModelStoreCatalog::get_items();
	if (selected_item < 0 || selected_item >= items.size()) {
		status_label->set_text("Select a model first.");
		return;
	}
	Node *scene_root = EditorInterface::get_singleton()->get_edited_scene_root();
	if (scene_root == nullptr) {
		status_label->set_text("No scene is open. Open or create a 3D scene first, then add the model.");
		return;
	}
	const ModelStoreCatalog::Item &item = items[selected_item];
	Node3D *model = ModelStoreCatalog::build_model(item);
	if (model == nullptr) {
		status_label->set_text("Could not build the model.");
		return;
	}

	// Unique name inside the scene.
	String unique_name = item.id;
	int suffix = 2;
	while (scene_root->get_node_or_null(NodePath(unique_name)) != nullptr) {
		unique_name = item.id + "_" + itos(suffix++);
	}
	model->set_name(unique_name);

	// Place in front of the editor camera, on the ground (y = 0).
	Vector3 target = Vector3(0, 0, 0);
	SubViewport *editor_vp = EditorInterface::get_singleton()->get_editor_viewport_3d(0);
	Camera3D *editor_cam = editor_vp != nullptr ? editor_vp->get_camera_3d() : nullptr;
	if (editor_cam != nullptr) {
		Vector3 forward = -editor_cam->get_global_transform().basis.get_axis(2);
		forward.y = 0;
		if (forward.length_squared() > 0.001f) {
			forward = forward.normalized();
			target = editor_cam->get_global_position() + forward * 6.0f;
			target.y = 0;
		}
	}
	model->set_position(target);

	EditorUndoRedoManager *undo = EditorInterface::get_singleton()->get_editor_undo_redo();
	undo->create_action("Insert 3D Model: " + item.name);
	undo->add_do_method(scene_root, "add_child", model);
	undo->add_do_method(model, "set_owner", scene_root);
	undo->add_undo_method(scene_root, "remove_child", model);
	undo->add_do_reference(model);
	undo->commit_action();
	EditorInterface::get_singleton()->mark_scene_as_unsaved();

	status_label->set_text(vformat("Added \"%s\" to the scene (undo with Ctrl+Z).", item.name));
}

void ModelStoreDialog::_on_close() {
	hide();
}

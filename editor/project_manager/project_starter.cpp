/**************************************************************************/
/*  project_starter.cpp                                                   */
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

#include "project_starter.h"

#include "core/io/dir_access.h"
#include "core/io/file_access.h"

bool ProjectStarter::_write_text_file(const String &p_path, const String &p_contents, String &r_error) {
	Ref<FileAccess> file = FileAccess::open(p_path, FileAccess::WRITE);
	if (file.is_null()) {
		r_error = "Could not write file: " + p_path;
		return false;
	}
	file->store_string(p_contents);
	file->close();
	return true;
}

bool ProjectStarter::scaffold_starter_game(const String &p_project_path, String &r_error) {
	Ref<DirAccess> dir = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);
	if (dir.is_null()) {
		r_error = "Could not access the filesystem.";
		return false;
	}
	if (!dir->dir_exists(p_project_path)) {
		r_error = "Project folder does not exist: " + p_project_path;
		return false;
	}
	// Never overwrite an existing project setup.
	if (dir->file_exists(p_project_path.path_join("main.tscn")) || dir->file_exists(p_project_path.path_join("player.tscn"))) {
		r_error = "Starter files already exist in this folder (main.tscn / player.tscn).";
		return false;
	}

	Error err = DirAccess::make_dir_recursive_absolute(p_project_path.path_join("scripts"));
	if (err != OK) {
		r_error = "Could not create the scripts folder.";
		return false;
	}

	if (!_write_text_file(p_project_path.path_join("scripts/player_controller.gd"), _player_controller_script(), r_error)) {
		return false;
	}
	if (!_write_text_file(p_project_path.path_join("scripts/virtual_joystick.gd"), _virtual_joystick_script(), r_error)) {
		return false;
	}
	if (!_write_text_file(p_project_path.path_join("scripts/touch_ui.gd"), _touch_ui_script(), r_error)) {
		return false;
	}
	if (!_write_text_file(p_project_path.path_join("player.tscn"), _player_scene(), r_error)) {
		return false;
	}
	if (!_write_text_file(p_project_path.path_join("main.tscn"), _main_scene(), r_error)) {
		return false;
	}
	if (!_write_text_file(p_project_path.path_join("README_STARTER.txt"), _readme_text(), r_error)) {
		return false;
	}
	return true;
}

String ProjectStarter::_player_controller_script() {
	return String(R"GDSCRIPT(extends CharacterBody3D
## Sohel Engine starter player controller.
## Keyboard: WASD / arrow keys to move, Space to jump, E to grab.
## Touch: left joystick to move, right joystick to look, GRAB button to grab.
## Movement is camera-relative and uses gravity from the project settings.
##
## External model: set `external_model` in the Inspector to a PackedScene
## (e.g. an imported .glb/.tscn character). The procedural humanoid is
## hidden and the model's own "grab" animation is used when it has one.

@export var speed := 5.0
@export var jump_velocity := 4.5
@export var look_sensitivity := 0.05
@export var external_model: PackedScene

const GRAVITY_FALLBACK := 9.8

var _grab_timer := 0.0
var _external_anim_player: AnimationPlayer = null

@onready var rig: Node3D = $Rig
@onready var camera_pivot: Node3D = $CameraPivot
@onready var camera: Camera3D = $CameraPivot/Camera3D
@onready var anim_player: AnimationPlayer = $AnimationPlayer

func _ready() -> void:
	_setup_external_model()
	_build_animations()

func _setup_external_model() -> void:
	if external_model == null:
		return
	var inst: Node = external_model.instantiate()
	if inst == null:
		return
	rig.add_child(inst)
	_hide_meshes(rig, inst)
	for node in inst.find_children("*", "AnimationPlayer", true, false):
		var ap := node as AnimationPlayer
		if ap != null and ap.has_animation("grab"):
			_external_anim_player = ap
			break

func _hide_meshes(p_node: Node, p_keep: Node) -> void:
	for child in p_node.get_children():
		if child == p_keep:
			continue
		if child is MeshInstance3D:
			(child as MeshInstance3D).visible = false
		_hide_meshes(child, p_keep)

func _build_animations() -> void:
	# The grab and walk animations are built here in code so the scene
	# file stays simple plain text. They animate the Rig pivots.
	if anim_player.has_animation("grab") and anim_player.has_animation("walk"):
		return

	# Grab: right arm reaches forward and comes back (0.8 s, plays once).
	var grab := Animation.new()
	grab.length = 0.8
	grab.loop_mode = Animation.LOOP_NONE
	var arm_track := grab.add_track(Animation.TYPE_VALUE)
	grab.track_set_path(arm_track, "Rig/ArmR_Pivot:rotation")
	grab.track_insert_key(arm_track, 0.0, Vector3.ZERO)
	grab.track_insert_key(arm_track, 0.35, Vector3(-1.5, 0.0, 0.0))
	grab.track_insert_key(arm_track, 0.8, Vector3.ZERO)
	anim_player.add_animation("grab", grab)

	# Walk: legs and arms swing opposite each other (0.6 s, loops).
	var walk := Animation.new()
	walk.length = 0.6
	walk.loop_mode = Animation.LOOP_LINEAR
	var leg_l := walk.add_track(Animation.TYPE_VALUE)
	walk.track_set_path(leg_l, "Rig/LegL_Pivot:rotation")
	walk.track_insert_key(leg_l, 0.0, Vector3(0.6, 0.0, 0.0))
	walk.track_insert_key(leg_l, 0.3, Vector3(-0.6, 0.0, 0.0))
	walk.track_insert_key(leg_l, 0.6, Vector3(0.6, 0.0, 0.0))
	var leg_r := walk.add_track(Animation.TYPE_VALUE)
	walk.track_set_path(leg_r, "Rig/LegR_Pivot:rotation")
	walk.track_insert_key(leg_r, 0.0, Vector3(-0.6, 0.0, 0.0))
	walk.track_insert_key(leg_r, 0.3, Vector3(0.6, 0.0, 0.0))
	walk.track_insert_key(leg_r, 0.6, Vector3(-0.6, 0.0, 0.0))
	var arm_l := walk.add_track(Animation.TYPE_VALUE)
	walk.track_set_path(arm_l, "Rig/ArmL_Pivot:rotation")
	walk.track_insert_key(arm_l, 0.0, Vector3(-0.45, 0.0, 0.0))
	walk.track_insert_key(arm_l, 0.3, Vector3(0.45, 0.0, 0.0))
	walk.track_insert_key(arm_l, 0.6, Vector3(-0.45, 0.0, 0.0))
	var arm_r := walk.add_track(Animation.TYPE_VALUE)
	walk.track_set_path(arm_r, "Rig/ArmR_Pivot:rotation")
	walk.track_insert_key(arm_r, 0.0, Vector3(0.45, 0.0, 0.0))
	walk.track_insert_key(arm_r, 0.3, Vector3(-0.45, 0.0, 0.0))
	walk.track_insert_key(arm_r, 0.6, Vector3(0.45, 0.0, 0.0))
	anim_player.add_animation("walk", walk)

func _physics_process(delta: float) -> void:
	var input_dir := Input.get_vector("ui_left", "ui_right", "ui_up", "ui_down")
	var touch_ui := get_tree().get_first_node_in_group("touch_ui")
	if touch_ui != null:
		input_dir += touch_ui.get_move_vector()
		_apply_look(touch_ui.get_look_vector() * look_sensitivity)
		if touch_ui.consume_grab():
			_do_grab()
	input_dir = input_dir.limit_length(1.0)

	var gravity: float = ProjectSettings.get_setting("physics/3d/default_gravity", GRAVITY_FALLBACK)
	if not is_on_floor():
		velocity.y -= gravity * delta

	if Input.is_action_just_pressed("ui_accept") and is_on_floor():
		velocity.y = jump_velocity

	if _wants_grab():
		_do_grab()

	# Camera-relative movement: forward is where the camera looks.
	var cam_basis := camera_pivot.global_transform.basis
	var direction := (cam_basis * Vector3(input_dir.x, 0.0, input_dir.y)).normalized()
	velocity.x = direction.x * speed
	velocity.z = direction.z * speed
	move_and_slide()

	if direction.length() > 0.1:
		var target_yaw := atan2(direction.x, direction.z)
		rig.rotation.y = lerp_angle(rig.rotation.y, target_yaw, 12.0 * delta)

	if _grab_timer > 0.0:
		_grab_timer -= delta
	elif direction.length() > 0.1 and is_on_floor():
		if anim_player.current_animation != "walk":
			anim_player.play("walk")
	elif anim_player.current_animation == "walk":
		anim_player.stop()

func _apply_look(p_look: Vector2) -> void:
	if p_look == Vector2.ZERO:
		return
	camera_pivot.rotation.y -= p_look.x
	camera.rotation.x = clampf(camera.rotation.x + p_look.y, -1.2, 1.2)

func _wants_grab() -> bool:
	if InputMap.has_action("grab"):
		return Input.is_action_just_pressed("grab")
	# Fallback when the project has no "grab" action defined.
	return Input.is_physical_key_pressed(KEY_E)

func _do_grab() -> void:
	if _external_anim_player != null:
		_external_anim_player.play("grab")
		return
	if anim_player.has_animation("grab"):
		anim_player.play("grab")
		_grab_timer = 0.8
)GDSCRIPT");
}

String ProjectStarter::_virtual_joystick_script() {
	return String(R"GDSCRIPT(extends Control
## Floating virtual joystick. Touch (or click with the mouse) anywhere
## inside this control and the stick base appears under the finger.
## `output` is a Vector2 in [-1, 1]: x = right, y = down.
## Release to recenter. Works with touch and with the mouse, so the
## controls can be tested on desktop too.

var output := Vector2.ZERO

var base_radius := 90.0
var knob_radius := 38.0

var _base_center := Vector2.ZERO
var _knob_offset := Vector2.ZERO
var _touch_index := -1
var _mouse_active := false

func _ready() -> void:
	mouse_filter = Control.MOUSE_FILTER_STOP
	_base_center = size * 0.5

func _notification(what: int) -> void:
	if what == NOTIFICATION_RESIZED:
		if _touch_index == -1 and not _mouse_active:
			_base_center = size * 0.5

func _draw() -> void:
	draw_circle(_base_center, base_radius, Color(1, 1, 1, 0.12))
	draw_arc(_base_center, base_radius, 0, TAU, 48, Color(1, 1, 1, 0.35), 3.0)
	draw_circle(_base_center + _knob_offset, knob_radius, Color(1, 1, 1, 0.35))
	draw_arc(_base_center + _knob_offset, knob_radius, 0, TAU, 48, Color(1, 1, 1, 0.6), 3.0)

func _gui_input(event: InputEvent) -> void:
	if event is InputEventScreenTouch:
		var touch := event as InputEventScreenTouch
		if touch.pressed and _touch_index == -1:
			_touch_index = touch.index
			_begin_drag(touch.position)
		elif not touch.pressed and touch.index == _touch_index:
			_touch_index = -1
			_end_drag()
	elif event is InputEventScreenDrag:
		var drag := event as InputEventScreenDrag
		if drag.index == _touch_index:
			_update_drag(drag.position)
	elif event is InputEventMouseButton:
		var mb := event as InputEventMouseButton
		if mb.button_index == MOUSE_BUTTON_LEFT:
			if mb.pressed and not _mouse_active:
				_mouse_active = true
				_begin_drag(mb.position)
			elif not mb.pressed and _mouse_active:
				_mouse_active = false
				_end_drag()
	elif event is InputEventMouseMotion:
		if _mouse_active:
			_update_drag((event as InputEventMouseMotion).position)

func _begin_drag(p_pos: Vector2) -> void:
	_base_center = p_pos.clamp(Vector2(base_radius, base_radius), size - Vector2(base_radius, base_radius))
	_knob_offset = Vector2.ZERO
	output = Vector2.ZERO
	queue_redraw()

func _update_drag(p_pos: Vector2) -> void:
	_knob_offset = p_pos - _base_center
	if _knob_offset.length() > base_radius:
		_knob_offset = _knob_offset.normalized() * base_radius
	output = _knob_offset / base_radius
	queue_redraw()

func _end_drag() -> void:
	_knob_offset = Vector2.ZERO
	output = Vector2.ZERO
	_base_center = size * 0.5
	queue_redraw()
)GDSCRIPT");
}

String ProjectStarter::_touch_ui_script() {
	return String(R"GDSCRIPT(extends CanvasLayer
## Touch UI for the starter game, built in code so no extra scene file
## is needed: left joystick moves, right joystick looks, GRAB button
## triggers the grab action. The player finds this node through the
## "touch_ui" group. The controls are semi-transparent and sit in the
## screen corners so they never cover the middle of the view.

const VirtualJoystick := preload("res://scripts/virtual_joystick.gd")

var _move_stick: VirtualJoystick
var _look_stick: VirtualJoystick
var _grab_queued := false

func _ready() -> void:
	add_to_group("touch_ui")
	layer = 10

	_move_stick = VirtualJoystick.new()
	_set_corner(_move_stick, 0.0, 1.0, Vector2(24, -284), Vector2(304, -24))
	add_child(_move_stick)

	_look_stick = VirtualJoystick.new()
	_set_corner(_look_stick, 1.0, 1.0, Vector2(-304, -284), Vector2(-24, -24))
	add_child(_look_stick)

	var grab_button := Button.new()
	grab_button.text = "GRAB"
	grab_button.add_theme_font_size_override("font_size", 28)
	_set_corner(grab_button, 1.0, 1.0, Vector2(-260, -430), Vector2(-140, -310))
	grab_button.pressed.connect(_on_grab_button_pressed)
	add_child(grab_button)

func _set_corner(p_control: Control, p_anchor_x: float, p_anchor_y: float, p_top_left: Vector2, p_bottom_right: Vector2) -> void:
	p_control.anchor_left = p_anchor_x
	p_control.anchor_top = p_anchor_y
	p_control.anchor_right = p_anchor_x
	p_control.anchor_bottom = p_anchor_y
	p_control.offset_left = p_top_left.x
	p_control.offset_top = p_top_left.y
	p_control.offset_right = p_bottom_right.x
	p_control.offset_bottom = p_bottom_right.y

func _on_grab_button_pressed() -> void:
	_grab_queued = true

func get_move_vector() -> Vector2:
	if _move_stick == null:
		return Vector2.ZERO
	return _move_stick.output

func get_look_vector() -> Vector2:
	if _look_stick == null:
		return Vector2.ZERO
	return _look_stick.output

func consume_grab() -> bool:
	var queued := _grab_queued
	_grab_queued = false
	return queued
)GDSCRIPT");
}

String ProjectStarter::_player_scene() {
	return String(R"TSCN([gd_scene load_steps=9 format=3]

[ext_resource type="Script" path="res://scripts/player_controller.gd" id="1_player"]

[sub_resource type="CapsuleShape3D" id="CapsuleShape3D_player"]
radius = 0.4
height = 1.8

[sub_resource type="CapsuleMesh" id="CapsuleMesh_body"]
radius = 0.35
height = 1.1

[sub_resource type="SphereMesh" id="SphereMesh_head"]
radius = 0.28
height = 0.56

[sub_resource type="BoxMesh" id="BoxMesh_arm"]
size = Vector3(0.18, 0.7, 0.18)

[sub_resource type="BoxMesh" id="BoxMesh_leg"]
size = Vector3(0.22, 0.8, 0.22)

[sub_resource type="StandardMaterial3D" id="Material_blue"]
albedo_color = Color(0.25, 0.55, 0.9, 1)

[sub_resource type="StandardMaterial3D" id="Material_skin"]
albedo_color = Color(0.96, 0.8, 0.65, 1)

[sub_resource type="StandardMaterial3D" id="Material_dark"]
albedo_color = Color(0.15, 0.18, 0.25, 1)

[node name="Player" type="CharacterBody3D"]
script = ExtResource("1_player")

[node name="CollisionShape3D" type="CollisionShape3D" parent="."]
transform = Transform3D(1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0.9, 0)
shape = SubResource("CapsuleShape3D_player")

[node name="Rig" type="Node3D" parent="."]

[node name="Body" type="MeshInstance3D" parent="Rig"]
transform = Transform3D(1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0)
mesh = SubResource("CapsuleMesh_body")
surface_material_override/0 = SubResource("Material_blue")

[node name="Head" type="MeshInstance3D" parent="Rig"]
transform = Transform3D(1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1.75, 0)
mesh = SubResource("SphereMesh_head")
surface_material_override/0 = SubResource("Material_skin")

[node name="ArmL_Pivot" type="Node3D" parent="Rig"]
transform = Transform3D(1, 0, 0, 0, 1, 0, 0, 0, 1, -0.5, 1.45, 0)

[node name="ArmL" type="MeshInstance3D" parent="Rig/ArmL_Pivot"]
transform = Transform3D(1, 0, 0, 0, 1, 0, 0, 0, 1, 0, -0.3, 0)
mesh = SubResource("BoxMesh_arm")
surface_material_override/0 = SubResource("Material_blue")

[node name="ArmR_Pivot" type="Node3D" parent="Rig"]
transform = Transform3D(1, 0, 0, 0, 1, 0, 0, 0, 1, 0.5, 1.45, 0)

[node name="ArmR" type="MeshInstance3D" parent="Rig/ArmR_Pivot"]
transform = Transform3D(1, 0, 0, 0, 1, 0, 0, 0, 1, 0, -0.3, 0)
mesh = SubResource("BoxMesh_arm")
surface_material_override/0 = SubResource("Material_blue")

[node name="LegL_Pivot" type="Node3D" parent="Rig"]
transform = Transform3D(1, 0, 0, 0, 1, 0, 0, 0, 1, -0.18, 0.85, 0)

[node name="LegL" type="MeshInstance3D" parent="Rig/LegL_Pivot"]
transform = Transform3D(1, 0, 0, 0, 1, 0, 0, 0, 1, 0, -0.4, 0)
mesh = SubResource("BoxMesh_leg")
surface_material_override/0 = SubResource("Material_dark")

[node name="LegR_Pivot" type="Node3D" parent="Rig"]
transform = Transform3D(1, 0, 0, 0, 1, 0, 0, 0, 1, 0.18, 0.85, 0)

[node name="LegR" type="MeshInstance3D" parent="Rig/LegR_Pivot"]
transform = Transform3D(1, 0, 0, 0, 1, 0, 0, 0, 1, 0, -0.4, 0)
mesh = SubResource("BoxMesh_leg")
surface_material_override/0 = SubResource("Material_dark")

[node name="CameraPivot" type="Node3D" parent="."]
transform = Transform3D(1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1.6, 0)

[node name="Camera3D" type="Camera3D" parent="CameraPivot"]
transform = Transform3D(1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 4)
fov = 70.0
current = true

[node name="AnimationPlayer" type="AnimationPlayer" parent="."]
)TSCN");
}

String ProjectStarter::_main_scene() {
	return String(R"TSCN([gd_scene load_steps=6 format=3]

[ext_resource type="Script" path="res://scripts/touch_ui.gd" id="1_touch"]
[ext_resource type="PackedScene" path="res://player.tscn" id="2_player"]

[sub_resource type="StandardMaterial3D" id="Material_floor"]
albedo_color = Color(1, 1, 1, 1)

[sub_resource type="BoxMesh" id="BoxMesh_floor"]
size = Vector3(20, 0.5, 20)

[sub_resource type="BoxShape3D" id="BoxShape_floor"]
size = Vector3(20, 0.5, 20)

[sub_resource type="Environment" id="Environment_main"]
background_mode = 1
background_color = Color(0.53, 0.74, 0.94, 1)
ambient_light_source = 2
ambient_light_color = Color(1, 1, 1, 1)
ambient_light_energy = 0.9

[node name="Main" type="Node3D"]

[node name="WorldEnvironment" type="WorldEnvironment" parent="."]
environment = SubResource("Environment_main")

[node name="Sun" type="DirectionalLight3D" parent="."]
rotation = Vector3(-0.872665, 0.523599, 0)
shadow_enabled = true
light_energy = 1.1

[node name="Floor" type="StaticBody3D" parent="."]

[node name="FloorMesh" type="MeshInstance3D" parent="Floor"]
transform = Transform3D(1, 0, 0, 0, 1, 0, 0, 0, 1, 0, -0.25, 0)
mesh = SubResource("BoxMesh_floor")
surface_material_override/0 = SubResource("Material_floor")

[node name="FloorCollision" type="CollisionShape3D" parent="Floor"]
transform = Transform3D(1, 0, 0, 0, 1, 0, 0, 0, 1, 0, -0.25, 0)
shape = SubResource("BoxShape_floor")

[node name="Player" parent="." instance=ExtResource("2_player")]
position = Vector3(0, 0.1, 0)

[node name="TouchUI" type="CanvasLayer" parent="."]
script = ExtResource("1_touch")
)TSCN");
}

String ProjectStarter::_readme_text() {
	return String(R"TXT(Sohel Engine - 3D Starter Game
============================

This project was created with the 3D starter game. Press Play (F5)
and the character is ready to move on the white floor right away.

Controls
--------
Keyboard:
  WASD / Arrow keys - move (camera-relative)
  Space             - jump
  E                 - grab (right hand reaches forward)
Touch (phone / tablet):
  Left joystick     - move
  Right joystick    - look around
  GRAB button       - grab

Files
-----
  main.tscn                  - Main scene (floor, sun, player, touch UI)
  player.tscn                - Player (CharacterBody3D + procedural humanoid)
  scripts/player_controller.gd - movement, gravity, camera, grab, animations
  scripts/virtual_joystick.gd  - floating touch joystick control
  scripts/touch_ui.gd          - builds the two joysticks + GRAB button

Use your own character model
----------------------------
Import a .glb / .gltf / .tscn character, open player.tscn, select the
Player node and set the `external_model` property in the Inspector.
The procedural humanoid is hidden and the model's own "grab" animation
is used when it has one.

Uncheck "Include 3D starter game" in the Create New Project dialog
to create an empty project instead.
)TXT");
}

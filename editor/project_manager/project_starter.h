/**************************************************************************/
/*  project_starter.h                                                     */
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

#include "core/string/ustring.h"

// Scaffolds a ready-to-play 3D starter game into a freshly created
// project folder: a white floor, a procedural humanoid character with
// grab/walk animations, movement + touch controls. All files are written
// as plain text (GDScript sources and .tscn scene text), so no project
// needs to be open while scaffolding runs from the Project Manager.
class ProjectStarter {
public:
	// Writes main.tscn, player.tscn, scripts/*.gd and a README into
	// p_project_path. Returns false (and fills r_error) on any failure,
	// including when starter files already exist, so an existing
	// project is never overwritten.
	static bool scaffold_starter_game(const String &p_project_path, String &r_error);

private:
	static bool _write_text_file(const String &p_path, const String &p_contents, String &r_error);

	static String _player_controller_script();
	static String _virtual_joystick_script();
	static String _touch_ui_script();
	static String _player_scene();
	static String _main_scene();
	static String _readme_text();
};

/**************************************************************************/
/*  model_store_catalog.h                                                 */
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

#include "core/math/color.h"
#include "core/string/ustring.h"
#include "core/templates/vector.h"

class Node3D;

// Data-driven catalog of the 100 built-in 3D models of the Sohel Engine
// Model Store. Every model is generated procedurally from Godot
// primitive meshes at runtime, so the store works fully offline and
// never depends on external files.
class ModelStoreCatalog {
public:
	enum Category {
		CAT_TREES,
		CAT_GROUND,
		CAT_ROADS,
		CAT_VEHICLES,
		CAT_BUILDINGS,
		CAT_CHARACTERS,
		CAT_COUNT,
	};

	enum Kind {
		KIND_TREE,
		KIND_BUSH,
		KIND_FLOWER,
		KIND_GROUND,
		KIND_HILL,
		KIND_ROCK,
		KIND_ROAD,
		KIND_BRIDGE,
		KIND_SIGN,
		KIND_LAMP,
		KIND_CAR,
		KIND_TRUCK,
		KIND_BIKE,
		KIND_BOAT,
		KIND_PLANE,
		KIND_HOUSE,
		KIND_TOWER,
		KIND_WINDMILL,
		KIND_PYRAMID,
		KIND_CHARACTER,
		KIND_ANIMAL,
		KIND_FURNITURE,
		KIND_PROP,
		KIND_FENCE,
		KIND_CHEST,
		KIND_FOUNTAIN,
		KIND_STATUE,
	};

	struct Item {
		String id;
		String name;
		int category = 0;
		int kind = 0;
		float size = 1.0f;
		Color color_a = Color(1, 1, 1);
		Color color_b = Color(1, 1, 1);
		int variant = 0;
		bool has_collision = true;
	};

	static const Vector<Item> &get_items();
	static String category_name(int p_category);

	// Builds a fresh Node3D hierarchy for the item. The caller owns it.
	// Returns nullptr only on an invalid item.
	static Node3D *build_model(const Item &p_item);
};

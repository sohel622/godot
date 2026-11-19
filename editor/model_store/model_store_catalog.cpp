/**************************************************************************/
/*  model_store_catalog.cpp                                               */
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

#include "model_store_catalog.h"

#include "scene/3d/physics/collision_shape_3d.h"
#include "scene/3d/mesh_instance_3d.h"
#include "scene/3d/node_3d.h"
#include "scene/3d/physics/static_body_3d.h"
#include "scene/resources/3d/box_shape_3d.h"
#include "scene/resources/3d/primitive_meshes.h"
#include "scene/resources/material.h"

String ModelStoreCatalog::category_name(int p_category) {
	switch (p_category) {
		case CAT_TREES:
			return "Trees & Foliage";
		case CAT_GROUND:
			return "Ground & Terrain";
		case CAT_ROADS:
			return "Roads & Streets";
		case CAT_VEHICLES:
			return "Vehicles";
		case CAT_BUILDINGS:
			return "Buildings & Houses";
		case CAT_CHARACTERS:
			return "Characters & Props";
		default:
			return "Other";
	}
}

const Vector<ModelStoreCatalog::Item> &ModelStoreCatalog::get_items() {
	static Vector<Item> items;
	if (!items.is_empty()) {
		return items;
	}

	// Trees & foliage (20).
	items.push_back(Item{ "oak_tree", "Oak Tree", CAT_TREES, KIND_TREE, 1.6f, Color(0.45f, 0.30f, 0.15f), Color(0.25f, 0.50f, 0.20f), 0, true });
	items.push_back(Item{ "pine_tree", "Pine Tree", CAT_TREES, KIND_TREE, 1.8f, Color(0.40f, 0.26f, 0.12f), Color(0.12f, 0.38f, 0.16f), 1, true });
	items.push_back(Item{ "birch_tree", "Birch Tree", CAT_TREES, KIND_TREE, 1.5f, Color(0.85f, 0.82f, 0.78f), Color(0.55f, 0.75f, 0.30f), 0, true });
	items.push_back(Item{ "palm_tree", "Palm Tree", CAT_TREES, KIND_TREE, 1.9f, Color(0.55f, 0.40f, 0.22f), Color(0.20f, 0.60f, 0.25f), 2, true });
	items.push_back(Item{ "willow_tree", "Willow Tree", CAT_TREES, KIND_TREE, 1.7f, Color(0.42f, 0.30f, 0.16f), Color(0.45f, 0.65f, 0.30f), 0, true });
	items.push_back(Item{ "maple_tree", "Maple Tree", CAT_TREES, KIND_TREE, 1.6f, Color(0.45f, 0.30f, 0.15f), Color(0.80f, 0.35f, 0.15f), 0, true });
	items.push_back(Item{ "cherry_tree", "Cherry Blossom", CAT_TREES, KIND_TREE, 1.6f, Color(0.45f, 0.30f, 0.15f), Color(0.95f, 0.70f, 0.80f), 0, true });
	items.push_back(Item{ "apple_tree", "Apple Tree", CAT_TREES, KIND_TREE, 1.5f, Color(0.45f, 0.30f, 0.15f), Color(0.30f, 0.55f, 0.22f), 3, true });
	items.push_back(Item{ "lemon_tree", "Lemon Tree", CAT_TREES, KIND_TREE, 1.4f, Color(0.45f, 0.30f, 0.15f), Color(0.35f, 0.60f, 0.25f), 3, true });
	items.push_back(Item{ "bamboo", "Bamboo", CAT_TREES, KIND_FLOWER, 1.6f, Color(0.35f, 0.65f, 0.25f), Color(0.25f, 0.55f, 0.20f), 8, true });
	items.push_back(Item{ "bush", "Bush", CAT_TREES, KIND_BUSH, 1.0f, Color(0.20f, 0.45f, 0.18f), Color(0.28f, 0.55f, 0.22f), 0, true });
	items.push_back(Item{ "hedge", "Hedge", CAT_TREES, KIND_BUSH, 1.2f, Color(0.18f, 0.42f, 0.16f), Color(0.22f, 0.50f, 0.20f), 1, true });
	items.push_back(Item{ "rose", "Rose Flower", CAT_TREES, KIND_FLOWER, 0.8f, Color(0.25f, 0.50f, 0.20f), Color(0.90f, 0.15f, 0.25f), 0, false });
	items.push_back(Item{ "tulip", "Tulip Flower", CAT_TREES, KIND_FLOWER, 0.8f, Color(0.25f, 0.50f, 0.20f), Color(0.95f, 0.60f, 0.15f), 1, false });
	items.push_back(Item{ "sunflower", "Sunflower", CAT_TREES, KIND_FLOWER, 1.0f, Color(0.30f, 0.55f, 0.20f), Color(0.95f, 0.80f, 0.10f), 2, false });
	items.push_back(Item{ "grass_patch", "Grass Patch", CAT_TREES, KIND_FLOWER, 0.9f, Color(0.30f, 0.60f, 0.22f), Color(0.45f, 0.75f, 0.30f), 3, false });
	items.push_back(Item{ "cactus", "Cactus", CAT_TREES, KIND_FLOWER, 1.1f, Color(0.20f, 0.55f, 0.25f), Color(0.30f, 0.65f, 0.30f), 4, true });
	items.push_back(Item{ "mushroom", "Mushroom", CAT_TREES, KIND_FLOWER, 0.9f, Color(0.90f, 0.85f, 0.75f), Color(0.85f, 0.20f, 0.15f), 5, false });
	items.push_back(Item{ "fern", "Fern", CAT_TREES, KIND_FLOWER, 0.9f, Color(0.20f, 0.50f, 0.22f), Color(0.30f, 0.62f, 0.28f), 6, false });
	items.push_back(Item{ "topiary", "Topiary", CAT_TREES, KIND_BUSH, 1.1f, Color(0.20f, 0.45f, 0.18f), Color(0.25f, 0.55f, 0.22f), 2, true });

	// Ground & terrain (12).
	items.push_back(Item{ "grass_ground", "Grass Ground", CAT_GROUND, KIND_GROUND, 5.0f, Color(0.35f, 0.65f, 0.25f), Color(0.30f, 0.55f, 0.22f), 0, true });
	items.push_back(Item{ "sand_ground", "Sand Ground", CAT_GROUND, KIND_GROUND, 5.0f, Color(0.90f, 0.80f, 0.55f), Color(0.85f, 0.72f, 0.45f), 0, true });
	items.push_back(Item{ "stone_ground", "Stone Ground", CAT_GROUND, KIND_GROUND, 5.0f, Color(0.55f, 0.55f, 0.58f), Color(0.45f, 0.45f, 0.48f), 0, true });
	items.push_back(Item{ "snow_ground", "Snow Ground", CAT_GROUND, KIND_GROUND, 5.0f, Color(0.95f, 0.97f, 1.0f), Color(0.88f, 0.92f, 0.98f), 0, true });
	items.push_back(Item{ "dirt_ground", "Dirt Ground", CAT_GROUND, KIND_GROUND, 5.0f, Color(0.55f, 0.40f, 0.25f), Color(0.48f, 0.34f, 0.20f), 0, true });
	items.push_back(Item{ "hill", "Hill", CAT_GROUND, KIND_HILL, 2.5f, Color(0.35f, 0.60f, 0.25f), Color(0.30f, 0.52f, 0.22f), 0, true });
	items.push_back(Item{ "mountain", "Mountain", CAT_GROUND, KIND_HILL, 3.5f, Color(0.45f, 0.45f, 0.48f), Color(0.95f, 0.96f, 1.0f), 1, true });
	items.push_back(Item{ "rock", "Rock", CAT_GROUND, KIND_ROCK, 1.0f, Color(0.55f, 0.55f, 0.58f), Color(0.45f, 0.45f, 0.48f), 0, true });
	items.push_back(Item{ "big_rock", "Big Rock", CAT_GROUND, KIND_ROCK, 1.8f, Color(0.50f, 0.50f, 0.53f), Color(0.40f, 0.40f, 0.43f), 1, true });
	items.push_back(Item{ "cliff", "Cliff", CAT_GROUND, KIND_HILL, 2.5f, Color(0.50f, 0.45f, 0.40f), Color(0.40f, 0.36f, 0.32f), 2, true });
	items.push_back(Item{ "island", "Island", CAT_GROUND, KIND_ROCK, 2.5f, Color(0.85f, 0.75f, 0.50f), Color(0.35f, 0.60f, 0.25f), 2, true });
	items.push_back(Item{ "pond", "Pond", CAT_GROUND, KIND_GROUND, 3.0f, Color(0.25f, 0.55f, 0.85f), Color(0.20f, 0.45f, 0.75f), 1, false });

	// Roads & streets (12).
	items.push_back(Item{ "straight_road", "Straight Road", CAT_ROADS, KIND_ROAD, 3.0f, Color(0.18f, 0.18f, 0.20f), Color(0.95f, 0.95f, 0.90f), 0, true });
	items.push_back(Item{ "curved_road", "L-Shaped Road", CAT_ROADS, KIND_ROAD, 3.0f, Color(0.18f, 0.18f, 0.20f), Color(0.95f, 0.95f, 0.90f), 1, true });
	items.push_back(Item{ "intersection", "Intersection", CAT_ROADS, KIND_ROAD, 3.0f, Color(0.18f, 0.18f, 0.20f), Color(0.95f, 0.95f, 0.90f), 2, true });
	items.push_back(Item{ "crosswalk", "Crosswalk", CAT_ROADS, KIND_ROAD, 2.5f, Color(0.18f, 0.18f, 0.20f), Color(0.95f, 0.95f, 0.90f), 3, true });
	items.push_back(Item{ "sidewalk", "Sidewalk", CAT_ROADS, KIND_ROAD, 3.0f, Color(0.70f, 0.70f, 0.72f), Color(0.60f, 0.60f, 0.62f), 4, true });
	items.push_back(Item{ "bridge", "Bridge", CAT_ROADS, KIND_BRIDGE, 3.0f, Color(0.60f, 0.45f, 0.30f), Color(0.50f, 0.38f, 0.25f), 0, true });
	items.push_back(Item{ "road_sign", "Road Sign", CAT_ROADS, KIND_SIGN, 1.0f, Color(0.70f, 0.70f, 0.72f), Color(0.85f, 0.15f, 0.12f), 0, true });
	items.push_back(Item{ "traffic_light", "Traffic Light", CAT_ROADS, KIND_LAMP, 1.2f, Color(0.15f, 0.15f, 0.16f), Color(0.90f, 0.15f, 0.12f), 1, true });
	items.push_back(Item{ "street_lamp", "Street Lamp", CAT_ROADS, KIND_LAMP, 1.2f, Color(0.20f, 0.20f, 0.22f), Color(0.95f, 0.85f, 0.45f), 0, true });
	items.push_back(Item{ "bus_stop", "Bus Stop", CAT_ROADS, KIND_SIGN, 1.4f, Color(0.25f, 0.45f, 0.75f), Color(0.95f, 0.95f, 0.95f), 1, true });
	items.push_back(Item{ "parking_lot", "Parking Lot", CAT_ROADS, KIND_ROAD, 3.0f, Color(0.22f, 0.22f, 0.24f), Color(0.95f, 0.95f, 0.90f), 5, true });
	items.push_back(Item{ "roundabout", "Roundabout", CAT_ROADS, KIND_ROAD, 3.0f, Color(0.18f, 0.18f, 0.20f), Color(0.35f, 0.60f, 0.25f), 6, true });

	// Vehicles (16).
	items.push_back(Item{ "sedan", "Sedan Car", CAT_VEHICLES, KIND_CAR, 1.0f, Color(0.80f, 0.15f, 0.15f), Color(0.10f, 0.12f, 0.18f), 0, true });
	items.push_back(Item{ "sports_car", "Sports Car", CAT_VEHICLES, KIND_CAR, 1.0f, Color(0.95f, 0.75f, 0.10f), Color(0.10f, 0.12f, 0.18f), 1, true });
	items.push_back(Item{ "truck", "Truck", CAT_VEHICLES, KIND_TRUCK, 1.2f, Color(0.25f, 0.45f, 0.75f), Color(0.85f, 0.85f, 0.88f), 0, true });
	items.push_back(Item{ "pickup", "Pickup Truck", CAT_VEHICLES, KIND_CAR, 1.1f, Color(0.25f, 0.55f, 0.30f), Color(0.10f, 0.12f, 0.18f), 6, true });
	items.push_back(Item{ "bus", "Bus", CAT_VEHICLES, KIND_TRUCK, 1.3f, Color(0.95f, 0.75f, 0.10f), Color(0.15f, 0.35f, 0.60f), 1, true });
	items.push_back(Item{ "van", "Van", CAT_VEHICLES, KIND_CAR, 1.1f, Color(0.85f, 0.85f, 0.88f), Color(0.10f, 0.12f, 0.18f), 5, true });
	items.push_back(Item{ "taxi", "Taxi", CAT_VEHICLES, KIND_CAR, 1.0f, Color(0.95f, 0.80f, 0.10f), Color(0.10f, 0.10f, 0.10f), 2, true });
	items.push_back(Item{ "police_car", "Police Car", CAT_VEHICLES, KIND_CAR, 1.0f, Color(0.95f, 0.95f, 0.97f), Color(0.10f, 0.20f, 0.70f), 3, true });
	items.push_back(Item{ "ambulance", "Ambulance", CAT_VEHICLES, KIND_CAR, 1.1f, Color(0.95f, 0.95f, 0.97f), Color(0.85f, 0.12f, 0.12f), 4, true });
	items.push_back(Item{ "fire_truck", "Fire Truck", CAT_VEHICLES, KIND_TRUCK, 1.2f, Color(0.85f, 0.12f, 0.10f), Color(0.90f, 0.90f, 0.90f), 2, true });
	items.push_back(Item{ "motorcycle", "Motorcycle", CAT_VEHICLES, KIND_BIKE, 0.9f, Color(0.15f, 0.15f, 0.18f), Color(0.80f, 0.15f, 0.15f), 0, true });
	items.push_back(Item{ "bicycle", "Bicycle", CAT_VEHICLES, KIND_BIKE, 0.9f, Color(0.20f, 0.50f, 0.80f), Color(0.85f, 0.85f, 0.88f), 1, true });
	items.push_back(Item{ "tractor", "Tractor", CAT_VEHICLES, KIND_TRUCK, 1.1f, Color(0.25f, 0.60f, 0.20f), Color(0.95f, 0.80f, 0.10f), 3, true });
	items.push_back(Item{ "train", "Train", CAT_VEHICLES, KIND_TRUCK, 1.4f, Color(0.20f, 0.35f, 0.65f), Color(0.90f, 0.90f, 0.92f), 4, true });
	items.push_back(Item{ "boat", "Boat", CAT_VEHICLES, KIND_BOAT, 1.2f, Color(0.85f, 0.85f, 0.88f), Color(0.70f, 0.30f, 0.15f), 0, true });
	items.push_back(Item{ "airplane", "Airplane", CAT_VEHICLES, KIND_PLANE, 1.3f, Color(0.90f, 0.90f, 0.92f), Color(0.20f, 0.40f, 0.75f), 0, true });

	// Buildings & houses (20).
	items.push_back(Item{ "house", "House", CAT_BUILDINGS, KIND_HOUSE, 1.2f, Color(0.90f, 0.80f, 0.65f), Color(0.70f, 0.25f, 0.15f), 0, true });
	items.push_back(Item{ "cottage", "Cottage", CAT_BUILDINGS, KIND_HOUSE, 1.0f, Color(0.95f, 0.90f, 0.80f), Color(0.45f, 0.30f, 0.18f), 1, true });
	items.push_back(Item{ "villa", "Villa", CAT_BUILDINGS, KIND_HOUSE, 1.6f, Color(0.95f, 0.95f, 0.95f), Color(0.20f, 0.45f, 0.70f), 2, true });
	items.push_back(Item{ "apartment", "Apartment Block", CAT_BUILDINGS, KIND_TOWER, 1.6f, Color(0.85f, 0.75f, 0.60f), Color(0.35f, 0.55f, 0.75f), 0, true });
	items.push_back(Item{ "skyscraper", "Skyscraper", CAT_BUILDINGS, KIND_TOWER, 2.2f, Color(0.55f, 0.65f, 0.80f), Color(0.25f, 0.35f, 0.55f), 1, true });
	items.push_back(Item{ "tower", "Tower", CAT_BUILDINGS, KIND_TOWER, 1.8f, Color(0.70f, 0.65f, 0.55f), Color(0.45f, 0.40f, 0.35f), 2, true });
	items.push_back(Item{ "shop", "Shop", CAT_BUILDINGS, KIND_HOUSE, 1.2f, Color(0.85f, 0.55f, 0.35f), Color(0.20f, 0.55f, 0.35f), 3, true });
	items.push_back(Item{ "cafe", "Cafe", CAT_BUILDINGS, KIND_HOUSE, 1.1f, Color(0.90f, 0.75f, 0.55f), Color(0.60f, 0.25f, 0.20f), 4, true });
	items.push_back(Item{ "school", "School", CAT_BUILDINGS, KIND_HOUSE, 1.8f, Color(0.90f, 0.80f, 0.60f), Color(0.80f, 0.20f, 0.15f), 5, true });
	items.push_back(Item{ "hospital", "Hospital", CAT_BUILDINGS, KIND_TOWER, 1.8f, Color(0.95f, 0.95f, 0.97f), Color(0.85f, 0.15f, 0.15f), 7, true });
	items.push_back(Item{ "church", "Church", CAT_BUILDINGS, KIND_TOWER, 1.6f, Color(0.92f, 0.90f, 0.85f), Color(0.50f, 0.40f, 0.30f), 8, true });
	items.push_back(Item{ "castle", "Castle", CAT_BUILDINGS, KIND_TOWER, 2.0f, Color(0.65f, 0.63f, 0.60f), Color(0.40f, 0.38f, 0.36f), 3, true });
	items.push_back(Item{ "windmill", "Windmill", CAT_BUILDINGS, KIND_WINDMILL, 1.6f, Color(0.92f, 0.90f, 0.85f), Color(0.70f, 0.45f, 0.25f), 0, true });
	items.push_back(Item{ "barn", "Barn", CAT_BUILDINGS, KIND_HOUSE, 1.3f, Color(0.75f, 0.25f, 0.15f), Color(0.95f, 0.95f, 0.95f), 6, true });
	items.push_back(Item{ "garage", "Garage", CAT_BUILDINGS, KIND_HOUSE, 1.1f, Color(0.75f, 0.75f, 0.78f), Color(0.25f, 0.35f, 0.55f), 7, true });
	items.push_back(Item{ "warehouse", "Warehouse", CAT_BUILDINGS, KIND_TOWER, 1.8f, Color(0.70f, 0.70f, 0.72f), Color(0.45f, 0.45f, 0.48f), 9, true });
	items.push_back(Item{ "office", "Office Building", CAT_BUILDINGS, KIND_TOWER, 1.9f, Color(0.60f, 0.65f, 0.75f), Color(0.20f, 0.30f, 0.50f), 5, true });
	items.push_back(Item{ "hotel", "Hotel", CAT_BUILDINGS, KIND_TOWER, 2.0f, Color(0.85f, 0.75f, 0.55f), Color(0.30f, 0.50f, 0.70f), 6, true });
	items.push_back(Item{ "lighthouse", "Lighthouse", CAT_BUILDINGS, KIND_TOWER, 1.8f, Color(0.95f, 0.95f, 0.97f), Color(0.85f, 0.15f, 0.12f), 4, true });
	items.push_back(Item{ "pyramid", "Pyramid", CAT_BUILDINGS, KIND_PYRAMID, 2.0f, Color(0.85f, 0.75f, 0.50f), Color(0.75f, 0.65f, 0.40f), 0, true });

	// Characters & props (20).
	items.push_back(Item{ "man", "Man", CAT_CHARACTERS, KIND_CHARACTER, 1.0f, Color(0.25f, 0.45f, 0.75f), Color(0.96f, 0.80f, 0.65f), 0, true });
	items.push_back(Item{ "woman", "Woman", CAT_CHARACTERS, KIND_CHARACTER, 1.0f, Color(0.85f, 0.35f, 0.55f), Color(0.96f, 0.80f, 0.65f), 1, true });
	items.push_back(Item{ "child", "Child", CAT_CHARACTERS, KIND_CHARACTER, 0.7f, Color(0.30f, 0.65f, 0.35f), Color(0.96f, 0.80f, 0.65f), 2, true });
	items.push_back(Item{ "knight", "Knight", CAT_CHARACTERS, KIND_CHARACTER, 1.1f, Color(0.65f, 0.65f, 0.70f), Color(0.85f, 0.15f, 0.15f), 3, true });
	items.push_back(Item{ "robot", "Robot", CAT_CHARACTERS, KIND_CHARACTER, 1.0f, Color(0.70f, 0.70f, 0.75f), Color(0.20f, 0.70f, 0.85f), 4, true });
	items.push_back(Item{ "alien", "Alien", CAT_CHARACTERS, KIND_CHARACTER, 0.9f, Color(0.35f, 0.75f, 0.30f), Color(0.20f, 0.60f, 0.25f), 5, true });
	items.push_back(Item{ "dog", "Dog", CAT_CHARACTERS, KIND_ANIMAL, 0.9f, Color(0.65f, 0.45f, 0.25f), Color(0.45f, 0.30f, 0.15f), 0, true });
	items.push_back(Item{ "cat", "Cat", CAT_CHARACTERS, KIND_ANIMAL, 0.8f, Color(0.85f, 0.80f, 0.75f), Color(0.90f, 0.85f, 0.80f), 1, true });
	items.push_back(Item{ "chair", "Chair", CAT_CHARACTERS, KIND_FURNITURE, 0.9f, Color(0.55f, 0.35f, 0.18f), Color(0.45f, 0.28f, 0.14f), 0, true });
	items.push_back(Item{ "table", "Table", CAT_CHARACTERS, KIND_FURNITURE, 1.0f, Color(0.60f, 0.40f, 0.20f), Color(0.50f, 0.32f, 0.16f), 1, true });
	items.push_back(Item{ "bench", "Bench", CAT_CHARACTERS, KIND_FURNITURE, 1.1f, Color(0.50f, 0.35f, 0.20f), Color(0.40f, 0.28f, 0.16f), 2, true });
	items.push_back(Item{ "lamp", "Table Lamp", CAT_CHARACTERS, KIND_LAMP, 0.8f, Color(0.25f, 0.25f, 0.28f), Color(0.95f, 0.85f, 0.45f), 2, true });
	items.push_back(Item{ "crate", "Crate", CAT_CHARACTERS, KIND_PROP, 0.9f, Color(0.65f, 0.48f, 0.25f), Color(0.55f, 0.40f, 0.20f), 0, true });
	items.push_back(Item{ "barrel", "Barrel", CAT_CHARACTERS, KIND_PROP, 0.9f, Color(0.55f, 0.35f, 0.15f), Color(0.45f, 0.28f, 0.12f), 1, true });
	items.push_back(Item{ "fence", "Fence", CAT_CHARACTERS, KIND_FENCE, 1.0f, Color(0.60f, 0.42f, 0.22f), Color(0.50f, 0.35f, 0.18f), 0, true });
	items.push_back(Item{ "treasure_chest", "Treasure Chest", CAT_CHARACTERS, KIND_CHEST, 0.9f, Color(0.55f, 0.35f, 0.15f), Color(0.95f, 0.80f, 0.15f), 0, true });
	items.push_back(Item{ "fountain", "Fountain", CAT_CHARACTERS, KIND_FOUNTAIN, 1.2f, Color(0.70f, 0.70f, 0.72f), Color(0.30f, 0.60f, 0.90f), 0, true });
	items.push_back(Item{ "statue", "Statue", CAT_CHARACTERS, KIND_STATUE, 1.2f, Color(0.75f, 0.75f, 0.78f), Color(0.60f, 0.60f, 0.63f), 0, true });
	items.push_back(Item{ "flag", "Flag", CAT_CHARACTERS, KIND_SIGN, 1.2f, Color(0.70f, 0.70f, 0.72f), Color(0.85f, 0.20f, 0.15f), 2, true });
	items.push_back(Item{ "torch", "Torch", CAT_CHARACTERS, KIND_LAMP, 0.9f, Color(0.45f, 0.30f, 0.15f), Color(0.95f, 0.55f, 0.10f), 3, true });

	return items;
}

// ---------------------------------------------------------------------------
// Procedural builders. Each one adds primitive meshes under p_root,
// using p_item.size / color_a / color_b / variant for variety.
// ---------------------------------------------------------------------------

static Ref<StandardMaterial3D> _mat(const Color &p_color) {
	Ref<StandardMaterial3D> mat;
	mat.instantiate();
	mat->set_albedo(p_color);
	return mat;
}

static MeshInstance3D *_box(Node *p_parent, const String &p_name, const Vector3 &p_pos, const Vector3 &p_size, const Color &p_color) {
	MeshInstance3D *mi = memnew(MeshInstance3D);
	mi->set_name(p_name);
	Ref<BoxMesh> mesh;
	mesh.instantiate();
	mesh->set_size(p_size);
	mesh->set_material(_mat(p_color));
	mi->set_mesh(mesh);
	mi->set_position(p_pos);
	p_parent->add_child(mi);
	return mi;
}

static MeshInstance3D *_cyl(Node *p_parent, const String &p_name, const Vector3 &p_pos, float p_top_radius, float p_bottom_radius, float p_height, const Color &p_color, int p_segments = 12) {
	MeshInstance3D *mi = memnew(MeshInstance3D);
	mi->set_name(p_name);
	Ref<CylinderMesh> mesh;
	mesh.instantiate();
	mesh->set_top_radius(p_top_radius);
	mesh->set_bottom_radius(p_bottom_radius);
	mesh->set_height(p_height);
	mesh->set_radial_segments(p_segments);
	mesh->set_material(_mat(p_color));
	mi->set_mesh(mesh);
	mi->set_position(p_pos);
	p_parent->add_child(mi);
	return mi;
}

static MeshInstance3D *_sphere(Node *p_parent, const String &p_name, const Vector3 &p_pos, float p_radius, const Color &p_color) {
	MeshInstance3D *mi = memnew(MeshInstance3D);
	mi->set_name(p_name);
	Ref<SphereMesh> mesh;
	mesh.instantiate();
	mesh->set_radius(p_radius);
	mesh->set_height(p_radius * 2.0f);
	mesh->set_material(_mat(p_color));
	mi->set_mesh(mesh);
	mi->set_position(p_pos);
	p_parent->add_child(mi);
	return mi;
}

static MeshInstance3D *_capsule(Node *p_parent, const String &p_name, const Vector3 &p_pos, float p_radius, float p_height, const Color &p_color) {
	MeshInstance3D *mi = memnew(MeshInstance3D);
	mi->set_name(p_name);
	Ref<CapsuleMesh> mesh;
	mesh.instantiate();
	mesh->set_radius(p_radius);
	mesh->set_height(p_height);
	mesh->set_material(_mat(p_color));
	mi->set_mesh(mesh);
	mi->set_position(p_pos);
	p_parent->add_child(mi);
	return mi;
}

static MeshInstance3D *_prism(Node *p_parent, const String &p_name, const Vector3 &p_pos, const Vector3 &p_size, const Color &p_color) {
	MeshInstance3D *mi = memnew(MeshInstance3D);
	mi->set_name(p_name);
	Ref<PrismMesh> mesh;
	mesh.instantiate();
	mesh->set_size(p_size);
	mesh->set_material(_mat(p_color));
	mi->set_mesh(mesh);
	mi->set_position(p_pos);
	p_parent->add_child(mi);
	return mi;
}

// Rotates a node around the Y axis (used for wheels, fronds, blades).
static void _yaw(Node3D *p_node, float p_degrees) {
	p_node->set_rotation(Vector3(0, Math::deg_to_rad(p_degrees), 0));
}

static void _build_tree(Node3D *p_root, const ModelStoreCatalog::Item &p_item) {
	const float s = p_item.size;
	if (p_item.variant == 1) {
		// Conifer: trunk + 3 stacked cones.
		_cyl(p_root, "Trunk", Vector3(0, 0.4f * s, 0), 0.12f * s, 0.18f * s, 0.8f * s, p_item.color_a);
		_cyl(p_root, "Leaves1", Vector3(0, 1.1f * s, 0), 0.05f * s, 0.75f * s, 0.9f * s, p_item.color_b);
		_cyl(p_root, "Leaves2", Vector3(0, 1.6f * s, 0), 0.05f * s, 0.55f * s, 0.8f * s, p_item.color_b);
		_cyl(p_root, "Leaves3", Vector3(0, 2.0f * s, 0), 0.02f * s, 0.35f * s, 0.7f * s, p_item.color_b);
	} else if (p_item.variant == 2) {
		// Palm: tall thin trunk + 5 fronds.
		_cyl(p_root, "Trunk", Vector3(0, 1.1f * s, 0), 0.10f * s, 0.16f * s, 2.2f * s, p_item.color_a);
		for (int i = 0; i < 5; i++) {
			MeshInstance3D *frond = _box(p_root, "Frond" + itos(i), Vector3(0, 2.25f * s, 0), Vector3(0.12f * s, 0.04f * s, 1.1f * s), p_item.color_b);
			frond->set_position(Vector3(Math::sin(Math::deg_to_rad(i * 72.0f)) * 0.45f * s, 2.25f * s, Math::cos(Math::deg_to_rad(i * 72.0f)) * 0.45f * s));
			_yaw(frond, i * 72.0f);
		}
		_sphere(p_root, "Coconuts", Vector3(0.15f * s, 2.1f * s, 0), 0.12f * s, Color(0.5f, 0.35f, 0.2f));
	} else {
		// Broadleaf: trunk + 3 leaf balls (+ fruit for variant 3).
		_cyl(p_root, "Trunk", Vector3(0, 0.6f * s, 0), 0.14f * s, 0.20f * s, 1.2f * s, p_item.color_a);
		_sphere(p_root, "Leaves1", Vector3(0, 1.6f * s, 0), 0.65f * s, p_item.color_b);
		_sphere(p_root, "Leaves2", Vector3(0.45f * s, 1.3f * s, 0.1f * s), 0.45f * s, p_item.color_b);
		_sphere(p_root, "Leaves3", Vector3(-0.4f * s, 1.35f * s, -0.1f * s), 0.45f * s, p_item.color_b);
		if (p_item.variant == 3) {
			_sphere(p_root, "Fruit1", Vector3(0.4f * s, 1.5f * s, 0.45f * s), 0.09f * s, Color(0.9f, 0.15f, 0.12f));
			_sphere(p_root, "Fruit2", Vector3(-0.35f * s, 1.6f * s, 0.4f * s), 0.09f * s, Color(0.95f, 0.85f, 0.15f));
			_sphere(p_root, "Fruit3", Vector3(0.1f * s, 1.25f * s, 0.55f * s), 0.09f * s, Color(0.9f, 0.15f, 0.12f));
		}
	}
}

static void _build_bush(Node3D *p_root, const ModelStoreCatalog::Item &p_item) {
	const float s = p_item.size;
	if (p_item.variant == 1) {
		// Hedge: long box.
		_box(p_root, "Hedge", Vector3(0, 0.45f * s, 0), Vector3(2.2f * s, 0.9f * s, 0.7f * s), p_item.color_b);
	} else if (p_item.variant == 2) {
		// Topiary: stacked spheres on a trunk.
		_cyl(p_root, "Trunk", Vector3(0, 0.3f * s, 0), 0.08f * s, 0.10f * s, 0.6f * s, p_item.color_a);
		_sphere(p_root, "Ball1", Vector3(0, 0.85f * s, 0), 0.35f * s, p_item.color_b);
		_sphere(p_root, "Ball2", Vector3(0, 1.35f * s, 0), 0.25f * s, p_item.color_b);
	} else {
		_sphere(p_root, "Bush1", Vector3(0, 0.35f * s, 0), 0.5f * s, p_item.color_b);
		_sphere(p_root, "Bush2", Vector3(0.4f * s, 0.3f * s, 0.1f * s), 0.35f * s, p_item.color_b);
		_sphere(p_root, "Bush3", Vector3(-0.35f * s, 0.3f * s, -0.1f * s), 0.35f * s, p_item.color_b);
	}
}

static void _build_flower(Node3D *p_root, const ModelStoreCatalog::Item &p_item) {
	const float s = p_item.size;
	switch (p_item.variant) {
		case 8: { // Bamboo: cluster of tall thin stalks.
			for (int i = 0; i < 4; i++) {
				float x = (i % 2 == 0 ? -0.25f : 0.25f) * s;
				float z = (i < 2 ? -0.2f : 0.2f) * s;
				_cyl(p_root, "Stalk" + itos(i), Vector3(x, 1.1f * s, z), 0.07f * s, 0.07f * s, 2.2f * s, p_item.color_a);
			}
		} break;
		case 4: { // Cactus: central column + arms.
			_capsule(p_root, "Body", Vector3(0, 0.7f * s, 0), 0.22f * s, 1.4f * s, p_item.color_a);
			_capsule(p_root, "ArmL", Vector3(-0.35f * s, 0.6f * s, 0), 0.10f * s, 0.5f * s, p_item.color_a);
			_capsule(p_root, "ArmR", Vector3(0.35f * s, 0.8f * s, 0), 0.10f * s, 0.5f * s, p_item.color_a);
		} break;
		case 5: { // Mushroom: stem + cap.
			_cyl(p_root, "Stem", Vector3(0, 0.25f * s, 0), 0.16f * s, 0.20f * s, 0.5f * s, p_item.color_a);
			_sphere(p_root, "Cap", Vector3(0, 0.55f * s, 0), 0.45f * s, p_item.color_b);
			_box(p_root, "CapBase", Vector3(0, 0.48f * s, 0), Vector3(0.85f * s, 0.08f * s, 0.85f * s), p_item.color_b);
		} break;
		case 3: { // Grass patch: thin cones.
			for (int i = 0; i < 6; i++) {
				float a = Math::deg_to_rad(i * 60.0f);
				_cyl(p_root, "Blade" + itos(i), Vector3(Math::cos(a) * 0.3f * s, 0.25f * s, Math::sin(a) * 0.3f * s), 0.01f * s, 0.08f * s, 0.5f * s, p_item.color_b);
			}
		} break;
		case 6: { // Fern: flat radiating leaves.
			for (int i = 0; i < 7; i++) {
				MeshInstance3D *leaf = _box(p_root, "Leaf" + itos(i), Vector3(0, 0.25f * s, 0), Vector3(0.10f * s, 0.03f * s, 0.9f * s), p_item.color_b);
				float a = Math::deg_to_rad(i * 51.4f);
				leaf->set_position(Vector3(Math::sin(a) * 0.35f * s, 0.25f * s, Math::cos(a) * 0.35f * s));
				_yaw(leaf, i * 51.4f);
			}
		} break;
		default: { // Rose / tulip / sunflower: stem + petals + center.
			_cyl(p_root, "Stem", Vector3(0, 0.35f * s, 0), 0.04f * s, 0.04f * s, 0.7f * s, p_item.color_a);
			for (int i = 0; i < 5; i++) {
				float a = Math::deg_to_rad(i * 72.0f);
				_sphere(p_root, "Petal" + itos(i), Vector3(Math::cos(a) * 0.16f * s, 0.75f * s, Math::sin(a) * 0.16f * s), 0.12f * s, p_item.color_b);
			}
			_sphere(p_root, "Center", Vector3(0, 0.78f * s, 0), 0.12f * s, p_item.variant == 2 ? Color(0.45f, 0.30f, 0.15f) : Color(0.95f, 0.85f, 0.20f));
		} break;
	}
}

static void _build_ground(Node3D *p_root, const ModelStoreCatalog::Item &p_item) {
	const float s = p_item.size;
	if (p_item.variant == 1) {
		// Pond: sand rim + blue water disc.
		_cyl(p_root, "Rim", Vector3(0, 0.05f * s, 0), 1.1f * s, 1.1f * s, 0.1f * s, Color(0.85f, 0.75f, 0.50f), 24);
		_cyl(p_root, "Water", Vector3(0, 0.08f * s, 0), 0.95f * s, 0.95f * s, 0.06f * s, p_item.color_a, 24);
	} else {
		_box(p_root, "Ground", Vector3(0, -0.2f * s, 0), Vector3(2.0f * s, 0.4f * s, 2.0f * s), p_item.color_a);
	}
}

static void _build_hill(Node3D *p_root, const ModelStoreCatalog::Item &p_item) {
	const float s = p_item.size;
	MeshInstance3D *hill = _sphere(p_root, "Hill", Vector3(0, 0, 0), 1.0f * s, p_item.color_a);
	hill->set_scale(Vector3(1.4f, p_item.variant == 1 ? 1.1f : 0.55f, 1.4f));
	hill->set_position(Vector3(0, p_item.variant == 1 ? 0.1f * s : -0.15f * s, 0));
	if (p_item.variant == 1) {
		// Snow cap.
		_cyl(p_root, "SnowCap", Vector3(0, 0.95f * s, 0), 0.05f * s, 0.45f * s, 0.35f * s, p_item.color_b);
	}
}

static void _build_rock(Node3D *p_root, const ModelStoreCatalog::Item &p_item) {
	const float s = p_item.size;
	if (p_item.variant == 2) {
		// Island: sand disc + tiny palm.
		_cyl(p_root, "Sand", Vector3(0, 0.1f * s, 0), 1.2f * s, 1.3f * s, 0.2f * s, p_item.color_a, 20);
		ModelStoreCatalog::Item palm{ "palm", "Palm", ModelStoreCatalog::CAT_TREES, ModelStoreCatalog::KIND_TREE, 0.8f * s, Color(0.55f, 0.40f, 0.22f), Color(0.20f, 0.60f, 0.25f), 2, false };
		_build_tree(p_root, palm);
	} else {
		MeshInstance3D *rock = _sphere(p_root, "Rock", Vector3(0, 0.35f * s, 0), 0.55f * s, p_item.color_a);
		rock->set_scale(Vector3(1.2f, 0.75f, 1.0f));
		rock->set_rotation(Vector3(0, 0.6f, 0.3f));
	}
}

static void _road_slab(Node3D *p_root, const String &p_name, const Vector3 &p_pos, const Vector3 &p_size, float p_yaw_deg, const Color &p_color) {
	MeshInstance3D *slab = _box(p_root, p_name, p_pos, p_size, p_color);
	_yaw(slab, p_yaw_deg);
}

static void _build_road(Node3D *p_root, const ModelStoreCatalog::Item &p_item) {
	const float s = p_item.size;
	const Color asphalt = p_item.color_a;
	const Color line = p_item.color_b;
	auto dashes = [&](const Vector3 &p_base, bool p_along_x) {
		for (int i = -2; i <= 2; i++) {
			Vector3 pos = p_base + (p_along_x ? Vector3(i * 0.9f * s, 0, 0) : Vector3(0, 0, i * 0.9f * s));
			_box(p_root, "Dash" + itos(i + 2), pos, p_along_x ? Vector3(0.45f * s, 0.02f, 0.12f * s) : Vector3(0.12f * s, 0.02f, 0.45f * s), line);
		}
	};
	switch (p_item.variant) {
		case 1: { // L-shaped road: two slabs at a right angle.
			_road_slab(p_root, "RoadA", Vector3(0, 0, -1.2f * s), Vector3(1.6f * s, 0.15f, 4.0f * s), 0, asphalt);
			_road_slab(p_root, "RoadB", Vector3(1.2f * s, 0, 0.8f * s), Vector3(4.0f * s, 0.15f, 1.6f * s), 0, asphalt);
			dashes(Vector3(0, 0.09f * s, -1.2f * s), false);
		} break;
		case 2: { // Intersection: cross slabs.
			_road_slab(p_root, "RoadNS", Vector3(0, 0, 0), Vector3(1.6f * s, 0.15f, 4.0f * s), 0, asphalt);
			_road_slab(p_root, "RoadEW", Vector3(0, 0.01f, 0), Vector3(4.0f * s, 0.15f, 1.6f * s), 0, asphalt);
		} break;
		case 3: { // Crosswalk: slab + white stripes.
			_road_slab(p_root, "Road", Vector3(0, 0, 0), Vector3(1.6f * s, 0.15f, 4.0f * s), 0, asphalt);
			for (int i = -2; i <= 2; i++) {
				_box(p_root, "Stripe" + itos(i + 2), Vector3(i * 0.32f * s, 0.09f * s, 0), Vector3(0.18f * s, 0.02f, 1.4f * s), line);
			}
		} break;
		case 4: { // Sidewalk: light slab with curb.
			_box(p_root, "Walk", Vector3(0, 0.1f * s, 0), Vector3(4.0f * s, 0.2f * s, 1.6f * s), asphalt);
			_box(p_root, "Curb", Vector3(0, 0.22f * s, 0.7f * s), Vector3(4.0f * s, 0.12f * s, 0.15f * s), line);
		} break;
		case 5: { // Parking lot: slab + bay lines.
			_box(p_root, "Lot", Vector3(0, 0, 0), Vector3(4.0f * s, 0.15f, 3.0f * s), asphalt);
			for (int i = -2; i <= 2; i++) {
				_box(p_root, "Bay" + itos(i + 2), Vector3(i * 0.8f * s, 0.09f * s, 0), Vector3(0.08f * s, 0.02f, 1.2f * s), line);
			}
		} break;
		case 6: { // Roundabout: ring (torus) + green center.
			MeshInstance3D *ring = memnew(MeshInstance3D);
			ring->set_name("Ring");
			Ref<TorusMesh> mesh;
			mesh.instantiate();
			mesh->set_inner_radius(0.9f * s);
			mesh->set_outer_radius(1.6f * s);
			mesh->set_material(_mat(asphalt));
			ring->set_mesh(mesh);
			ring->set_position(Vector3(0, 0.08f * s, 0));
			p_root->add_child(ring);
			_cyl(p_root, "Center", Vector3(0, 0.06f * s, 0), 0.85f * s, 0.85f * s, 0.12f * s, line, 20);
		} break;
		default: { // Straight road with dashes.
			_road_slab(p_root, "Road", Vector3(0, 0, 0), Vector3(1.6f * s, 0.15f, 4.0f * s), 0, asphalt);
			dashes(Vector3(0, 0.09f * s, 0), false);
		} break;
	}
}

static void _build_bridge(Node3D *p_root, const ModelStoreCatalog::Item &p_item) {
	const float s = p_item.size;
	_box(p_root, "Deck", Vector3(0, 0.5f * s, 0), Vector3(1.8f * s, 0.2f * s, 5.0f * s), p_item.color_a);
	_box(p_root, "RailL", Vector3(-0.85f * s, 0.95f * s, 0), Vector3(0.12f * s, 0.7f * s, 5.0f * s), p_item.color_b);
	_box(p_root, "RailR", Vector3(0.85f * s, 0.95f * s, 0), Vector3(0.12f * s, 0.7f * s, 5.0f * s), p_item.color_b);
	_cyl(p_root, "PostL", Vector3(-0.6f * s, 0, -1.8f * s), 0.18f * s, 0.22f * s, 0.5f * s, p_item.color_b);
	_cyl(p_root, "PostR", Vector3(0.6f * s, 0, -1.8f * s), 0.18f * s, 0.22f * s, 0.5f * s, p_item.color_b);
	_cyl(p_root, "PostL2", Vector3(-0.6f * s, 0, 1.8f * s), 0.18f * s, 0.22f * s, 0.5f * s, p_item.color_b);
	_cyl(p_root, "PostR2", Vector3(0.6f * s, 0, 1.8f * s), 0.18f * s, 0.22f * s, 0.5f * s, p_item.color_b);
}

static void _build_sign(Node3D *p_root, const ModelStoreCatalog::Item &p_item) {
	const float s = p_item.size;
	_cyl(p_root, "Post", Vector3(0, 0.9f * s, 0), 0.05f * s, 0.05f * s, 1.8f * s, p_item.color_a);
	if (p_item.variant == 2) {
		// Flag: pole + waving cloth.
		_box(p_root, "Cloth", Vector3(0.45f * s, 1.55f * s, 0), Vector3(0.9f * s, 0.55f * s, 0.03f), p_item.color_b);
	} else if (p_item.variant == 1) {
		// Bus stop: post + shelter roof + bench.
		_box(p_root, "Board", Vector3(0, 1.9f * s, 0), Vector3(0.9f * s, 0.6f * s, 0.06f), p_item.color_b);
		_box(p_root, "Roof", Vector3(0, 2.2f * s, -0.3f * s), Vector3(1.6f * s, 0.08f * s, 1.0f * s), p_item.color_a);
	} else {
		// Road sign: octagon-ish board (cylinder, 8 segments).
		_cyl(p_root, "Board", Vector3(0, 1.9f * s, 0), 0.4f * s, 0.4f * s, 0.06f, p_item.color_b, 8);
	}
}

static void _build_lamp(Node3D *p_root, const ModelStoreCatalog::Item &p_item) {
	const float s = p_item.size;
	if (p_item.variant == 1) {
		// Traffic light: pole + box + 3 lights.
		_cyl(p_root, "Pole", Vector3(0, 1.1f * s, 0), 0.07f * s, 0.09f * s, 2.2f * s, p_item.color_a);
		_box(p_root, "Housing", Vector3(0, 2.4f * s, 0), Vector3(0.35f * s, 0.9f * s, 0.3f * s), Color(0.12f, 0.12f, 0.13f));
		_sphere(p_root, "Red", Vector3(0, 2.68f * s, 0.16f * s), 0.10f * s, Color(0.9f, 0.12f, 0.10f));
		_sphere(p_root, "Yellow", Vector3(0, 2.4f * s, 0.16f * s), 0.10f * s, Color(0.95f, 0.80f, 0.10f));
		_sphere(p_root, "Green", Vector3(0, 2.12f * s, 0.16f * s), 0.10f * s, Color(0.15f, 0.75f, 0.25f));
	} else if (p_item.variant == 3) {
		// Torch: wooden pole + flame cone.
		_cyl(p_root, "Pole", Vector3(0, 0.6f * s, 0), 0.06f * s, 0.08f * s, 1.2f * s, p_item.color_a);
		_cyl(p_root, "Flame", Vector3(0, 1.4f * s, 0), 0.02f * s, 0.18f * s, 0.5f * s, p_item.color_b);
		_sphere(p_root, "Ember", Vector3(0, 1.25f * s, 0), 0.12f * s, Color(0.95f, 0.65f, 0.10f));
	} else if (p_item.variant == 2) {
		// Table lamp: base + stem + shade.
		_cyl(p_root, "Base", Vector3(0, 0.05f * s, 0), 0.25f * s, 0.28f * s, 0.1f * s, p_item.color_a);
		_cyl(p_root, "Stem", Vector3(0, 0.35f * s, 0), 0.04f * s, 0.04f * s, 0.6f * s, p_item.color_a);
		_cyl(p_root, "Shade", Vector3(0, 0.75f * s, 0), 0.18f * s, 0.32f * s, 0.35f * s, p_item.color_b);
	} else {
		// Street lamp: pole + arm + glowing head.
		_cyl(p_root, "Pole", Vector3(0, 1.4f * s, 0), 0.06f * s, 0.09f * s, 2.8f * s, p_item.color_a);
		_box(p_root, "Arm", Vector3(0.35f * s, 2.75f * s, 0), Vector3(0.7f * s, 0.08f * s, 0.08f * s), p_item.color_a);
		_sphere(p_root, "Bulb", Vector3(0.7f * s, 2.65f * s, 0), 0.12f * s, p_item.color_b);
	}
}

static void _wheel(Node3D *p_root, const String &p_name, const Vector3 &p_pos, float p_radius, float p_width) {
	MeshInstance3D *wheel = _cyl(p_root, p_name, p_pos, p_radius, p_radius, p_width, Color(0.08f, 0.08f, 0.09f), 16);
	wheel->set_rotation(Vector3(0, 0, Math::deg_to_rad(90.0f)));
}

static void _build_car(Node3D *p_root, const ModelStoreCatalog::Item &p_item) {
	const float s = p_item.size;
	const float len = (p_item.variant == 1) ? 2.4f : 2.0f;
	const float body_h = (p_item.variant == 5) ? 0.75f : 0.5f;
	// Body.
	_box(p_root, "Body", Vector3(0, 0.35f * s, 0), Vector3(0.95f * s, body_h * s, len * s), p_item.color_a);
	// Cabin (not for van/pickup bed variants).
	if (p_item.variant != 5 && p_item.variant != 6) {
		_box(p_root, "Cabin", Vector3(0, 0.75f * s, -0.15f * s), Vector3(0.8f * s, 0.45f * s, 1.1f * s), p_item.color_b);
	} else if (p_item.variant == 6) {
		_box(p_root, "Bed", Vector3(0, 0.45f * s, -0.6f * s), Vector3(0.9f * s, 0.25f * s, 0.9f * s), p_item.color_b);
	}
	// Lights: white front, red rear.
	_box(p_root, "LightF", Vector3(0.3f * s, 0.35f * s, len * 0.5f * s), Vector3(0.2f * s, 0.1f * s, 0.03f), Color(0.95f, 0.95f, 0.85f));
	_box(p_root, "LightR", Vector3(-0.3f * s, 0.35f * s, -len * 0.5f * s), Vector3(0.2f * s, 0.1f * s, 0.03f), Color(0.85f, 0.12f, 0.10f));
	// Wheels.
	_wheel(p_root, "WheelFL", Vector3(-0.48f * s, 0.22f * s, 0.65f * s), 0.22f * s, 0.12f);
	_wheel(p_root, "WheelFR", Vector3(0.48f * s, 0.22f * s, 0.65f * s), 0.22f * s, 0.12f);
	_wheel(p_root, "WheelRL", Vector3(-0.48f * s, 0.22f * s, -0.65f * s), 0.22f * s, 0.12f);
	_wheel(p_root, "WheelRR", Vector3(0.48f * s, 0.22f * s, -0.65f * s), 0.22f * s, 0.12f);
	if (p_item.variant == 3) {
		// Police light bar.
		_box(p_root, "LightBar", Vector3(0, 1.05f * s, -0.15f * s), Vector3(0.6f * s, 0.08f * s, 0.2f * s), Color(0.15f, 0.25f, 0.85f));
	} else if (p_item.variant == 4) {
		_box(p_root, "Cross", Vector3(0, 0.8f * s, 0.6f * s), Vector3(0.3f * s, 0.3f * s, 0.03f), Color(0.85f, 0.12f, 0.12f));
	}
}

static void _build_truck(Node3D *p_root, const ModelStoreCatalog::Item &p_item) {
	const float s = p_item.size;
	if (p_item.variant == 4) {
		// Train: loco + 2 cars.
		for (int i = 0; i < 3; i++) {
			float z = -i * 1.9f * s;
			_box(p_root, "Car" + itos(i), Vector3(0, 0.6f * s, z), Vector3(0.9f * s, 0.8f * s, 1.7f * s), i == 0 ? p_item.color_a : p_item.color_b);
			_wheel(p_root, "WheelL" + itos(i), Vector3(-0.45f * s, 0.22f * s, z), 0.22f * s, 0.12f);
			_wheel(p_root, "WheelR" + itos(i), Vector3(0.45f * s, 0.22f * s, z), 0.22f * s, 0.12f);
		}
		return;
	}
	// Cab + cargo.
	_box(p_root, "Cab", Vector3(0, 0.55f * s, 1.15f * s), Vector3(0.95f * s, 0.9f * s, 0.9f * s), p_item.color_a);
	_box(p_root, "Windshield", Vector3(0, 0.7f * s, 1.62f * s), Vector3(0.8f * s, 0.4f * s, 0.03f), Color(0.10f, 0.12f, 0.18f));
	float cargo_len = (p_item.variant == 1) ? 2.6f : 1.9f;
	float cargo_h = (p_item.variant == 1) ? 1.1f : 1.3f;
	_box(p_root, "Cargo", Vector3(0, 0.75f * s, -0.55f * s), Vector3(1.0f * s, cargo_h * s, cargo_len * s), p_item.color_b);
	_wheel(p_root, "WheelFL", Vector3(-0.5f * s, 0.25f * s, 1.2f * s), 0.25f * s, 0.14f);
	_wheel(p_root, "WheelFR", Vector3(0.5f * s, 0.25f * s, 1.2f * s), 0.25f * s, 0.14f);
	_wheel(p_root, "WheelRL", Vector3(-0.5f * s, 0.25f * s, -1.0f * s), 0.25f * s, 0.14f);
	_wheel(p_root, "WheelRR", Vector3(0.5f * s, 0.25f * s, -1.0f * s), 0.25f * s, 0.14f);
	if (p_item.variant == 1) {
		// Bus windows.
		for (int i = 0; i < 4; i++) {
			_box(p_root, "Window" + itos(i), Vector3(0.51f * s, 0.95f * s, -0.2f * s - i * 0.45f * s), Vector3(0.02f, 0.4f * s, 0.3f * s), Color(0.10f, 0.12f, 0.18f));
		}
	} else if (p_item.variant == 3) {
		// Tractor: big rear wheels, exhaust.
		_wheel(p_root, "WheelBigL", Vector3(-0.55f * s, 0.35f * s, -1.0f * s), 0.38f * s, 0.18f);
		_wheel(p_root, "WheelBigR", Vector3(0.55f * s, 0.35f * s, -1.0f * s), 0.38f * s, 0.18f);
		_cyl(p_root, "Exhaust", Vector3(0.3f * s, 1.3f * s, 1.1f * s), 0.05f * s, 0.05f * s, 0.7f * s, Color(0.2f, 0.2f, 0.22f));
	}
}

static void _build_bike(Node3D *p_root, const ModelStoreCatalog::Item &p_item) {
	const float s = p_item.size;
	// Two wheels + frame.
	MeshInstance3D *w1 = _cyl(p_root, "WheelF", Vector3(0, 0.28f * s, 0.75f * s), 0.28f * s, 0.28f * s, 0.06f, Color(0.08f, 0.08f, 0.09f), 16);
	w1->set_rotation(Vector3(0, 0, Math::deg_to_rad(90.0f)));
	MeshInstance3D *w2 = _cyl(p_root, "WheelR", Vector3(0, 0.28f * s, -0.75f * s), 0.28f * s, 0.28f * s, 0.06f, Color(0.08f, 0.08f, 0.09f), 16);
	w2->set_rotation(Vector3(0, 0, Math::deg_to_rad(90.0f)));
	_box(p_root, "Frame", Vector3(0, 0.45f * s, 0), Vector3(0.08f * s, 0.08f * s, 1.5f * s), p_item.color_a);
	_box(p_root, "Seat", Vector3(0, 0.65f * s, -0.5f * s), Vector3(0.15f * s, 0.06f * s, 0.35f * s), Color(0.15f, 0.15f, 0.16f));
	_box(p_root, "Handle", Vector3(0, 0.7f * s, 0.7f * s), Vector3(0.5f * s, 0.06f * s, 0.06f), Color(0.15f, 0.15f, 0.16f));
	if (p_item.variant == 0) {
		// Motorcycle: tank + engine block.
		_box(p_root, "Tank", Vector3(0, 0.6f * s, 0.15f * s), Vector3(0.35f * s, 0.25f * s, 0.6f * s), p_item.color_b);
		_box(p_root, "Engine", Vector3(0, 0.35f * s, 0), Vector3(0.3f * s, 0.3f * s, 0.5f * s), Color(0.25f, 0.25f, 0.28f));
	}
}

static void _build_boat(Node3D *p_root, const ModelStoreCatalog::Item &p_item) {
	const float s = p_item.size;
	_box(p_root, "Hull", Vector3(0, 0.25f * s, 0), Vector3(1.0f * s, 0.4f * s, 2.4f * s), p_item.color_b);
	MeshInstance3D *bow = _cyl(p_root, "Bow", Vector3(0, 0.25f * s, 1.35f * s), 0.02f, 0.5f * s, 0.6f * s, p_item.color_b, 4);
	bow->set_rotation(Vector3(Math::deg_to_rad(90.0f), Math::deg_to_rad(45.0f), 0));
	_box(p_root, "Cabin", Vector3(0, 0.6f * s, -0.3f * s), Vector3(0.6f * s, 0.35f * s, 0.8f * s), p_item.color_a);
	_cyl(p_root, "Mast", Vector3(0, 1.1f * s, 0.3f * s), 0.04f * s, 0.05f * s, 1.2f * s, Color(0.45f, 0.30f, 0.15f));
	_prism(p_root, "Sail", Vector3(0, 1.15f * s, 0.3f * s), Vector3(0.06f, 0.9f * s, 0.8f * s), Color(0.95f, 0.95f, 0.95f));
}

static void _build_plane(Node3D *p_root, const ModelStoreCatalog::Item &p_item) {
	const float s = p_item.size;
	MeshInstance3D *body = _cyl(p_root, "Body", Vector3(0, 0.7f * s, 0), 0.28f * s, 0.28f * s, 2.6f * s, p_item.color_a, 16);
	body->set_rotation(Vector3(Math::deg_to_rad(90.0f), 0, 0));
	_box(p_root, "WingL", Vector3(-1.1f * s, 0.7f * s, 0), Vector3(1.6f * s, 0.06f, 0.7f * s), p_item.color_b);
	_box(p_root, "WingR", Vector3(1.1f * s, 0.7f * s, 0), Vector3(1.6f * s, 0.06f, 0.7f * s), p_item.color_b);
	_box(p_root, "Tail", Vector3(0, 1.0f * s, -1.25f * s), Vector3(0.06f, 0.5f * s, 0.4f * s), p_item.color_b);
	_box(p_root, "TailWing", Vector3(0, 0.85f * s, -1.25f * s), Vector3(1.0f * s, 0.05f, 0.35f * s), p_item.color_b);
	_sphere(p_root, "Nose", Vector3(0, 0.7f * s, 1.35f * s), 0.28f * s, p_item.color_b);
}

static void _build_house(Node3D *p_root, const ModelStoreCatalog::Item &p_item) {
	const float s = p_item.size;
	const float w = (p_item.variant == 2 || p_item.variant == 5) ? 2.4f : 1.8f;
	_box(p_root, "Walls", Vector3(0, 0.6f * s, 0), Vector3(w * s, 1.2f * s, 1.6f * s), p_item.color_a);
	_prism(p_root, "Roof", Vector3(0, 1.55f * s, 0), Vector3((w + 0.3f) * s, 0.7f * s, 1.9f * s), p_item.color_b);
	_box(p_root, "Door", Vector3(0, 0.35f * s, 0.82f * s), Vector3(0.4f * s, 0.7f * s, 0.05f), Color(0.45f, 0.28f, 0.14f));
	_box(p_root, "WindowL", Vector3(-0.55f * s, 0.7f * s, 0.82f * s), Vector3(0.4f * s, 0.4f * s, 0.05f), Color(0.55f, 0.75f, 0.90f));
	_box(p_root, "WindowR", Vector3(0.55f * s, 0.7f * s, 0.82f * s), Vector3(0.4f * s, 0.4f * s, 0.05f), Color(0.55f, 0.75f, 0.90f));
	if (p_item.variant == 5) {
		_box(p_root, "Sign", Vector3(0, 1.1f * s, 0.85f * s), Vector3(1.2f * s, 0.3f * s, 0.05f), Color(0.85f, 0.15f, 0.12f));
	}
}

static void _build_tower(Node3D *p_root, const ModelStoreCatalog::Item &p_item) {
	const float s = p_item.size;
	const float h = (p_item.variant == 1) ? 4.5f : 3.0f;
	if (p_item.variant == 2 || p_item.variant == 4) {
		// Round tower / lighthouse.
		_cyl(p_root, "Tower", Vector3(0, h * 0.5f * s, 0), 0.55f * s, 0.7f * s, h * s, p_item.color_a, 16);
		_cyl(p_root, "Top", Vector3(0, (h + 0.25f) * s, 0), 0.7f * s, 0.7f * s, 0.5f * s, p_item.color_b, 16);
		if (p_item.variant == 4) {
			_sphere(p_root, "Light", Vector3(0, (h + 0.6f) * s, 0), 0.25f * s, Color(0.95f, 0.85f, 0.35f));
		}
		return;
	}
	_box(p_root, "Tower", Vector3(0, h * 0.5f * s, 0), Vector3(1.4f * s, h * s, 1.4f * s), p_item.color_a);
	int floors = (p_item.variant == 1) ? 6 : 4;
	for (int i = 0; i < floors; i++) {
		float y = (0.6f + i * (h - 1.0f) / MAX(1, floors - 1)) * s;
		_box(p_root, "WindowF" + itos(i), Vector3(-0.35f * s, y, 0.71f * s), Vector3(0.4f * s, 0.4f * s, 0.03f), p_item.color_b);
		_box(p_root, "WindowF2" + itos(i), Vector3(0.35f * s, y, 0.71f * s), Vector3(0.4f * s, 0.4f * s, 0.03f), p_item.color_b);
	}
	if (p_item.variant == 3) {
		// Castle crenellations.
		for (int i = 0; i < 4; i++) {
			float a = Math::deg_to_rad(i * 90.0f + 45.0f);
			_box(p_root, "Cren" + itos(i), Vector3(Math::sin(a) * 0.6f * s, (h + 0.25f) * s, Math::cos(a) * 0.6f * s), Vector3(0.25f * s, 0.5f * s, 0.25f * s), p_item.color_a);
		}
	} else if (p_item.variant == 7) {
		_box(p_root, "Cross", Vector3(0, (h + 0.5f) * s, 0.71f * s), Vector3(0.3f * s, 0.5f * s, 0.05f), p_item.color_b);
	}
}

static void _build_windmill(Node3D *p_root, const ModelStoreCatalog::Item &p_item) {
	const float s = p_item.size;
	_cyl(p_root, "Tower", Vector3(0, 1.2f * s, 0), 0.6f * s, 0.9f * s, 2.4f * s, p_item.color_a, 12);
	_cyl(p_root, "Cap", Vector3(0, 2.55f * s, 0), 0.05f * s, 0.65f * s, 0.5f * s, p_item.color_b, 12);
	Node3D *hub = memnew(Node3D);
	hub->set_name("Blades");
	hub->set_position(Vector3(0, 2.3f * s, 0.75f * s));
	p_root->add_child(hub);
	for (int i = 0; i < 4; i++) {
		MeshInstance3D *blade = _box(hub, "Blade" + itos(i), Vector3(0, 0, 0), Vector3(0.18f * s, 1.3f * s, 0.04f), Color(0.95f, 0.95f, 0.95f));
		blade->set_rotation(Vector3(0, 0, Math::deg_to_rad(i * 90.0f)));
		blade->set_position(Vector3(Math::sin(Math::deg_to_rad(i * 90.0f)) * 0.65f * s, Math::cos(Math::deg_to_rad(i * 90.0f)) * 0.65f * s, 0));
	}
}

static void _build_pyramid(Node3D *p_root, const ModelStoreCatalog::Item &p_item) {
	const float s = p_item.size;
	MeshInstance3D *pyramid = _cyl(p_root, "Pyramid", Vector3(0, 0.9f * s, 0), 0.02f * s, 1.6f * s, 1.8f * s, p_item.color_a, 4);
	_yaw(pyramid, 45.0f);
}

static void _build_character(Node3D *p_root, const ModelStoreCatalog::Item &p_item) {
	const float s = p_item.size;
	// Same proportions as the starter player, tinted per variant.
	_capsule(p_root, "Body", Vector3(0, 1.0f * s, 0), 0.35f * s, 1.1f * s, p_item.color_a);
	_sphere(p_root, "Head", Vector3(0, 1.75f * s, 0), 0.28f * s, p_item.color_b);
	_box(p_root, "ArmL", Vector3(-0.5f * s, 1.15f * s, 0), Vector3(0.18f * s, 0.7f * s, 0.18f * s), p_item.color_a);
	_box(p_root, "ArmR", Vector3(0.5f * s, 1.15f * s, 0), Vector3(0.18f * s, 0.7f * s, 0.18f * s), p_item.color_a);
	_box(p_root, "LegL", Vector3(-0.18f * s, 0.45f * s, 0), Vector3(0.22f * s, 0.8f * s, 0.22f * s), Color(0.15f, 0.18f, 0.25f));
	_box(p_root, "LegR", Vector3(0.18f * s, 0.45f * s, 0), Vector3(0.22f * s, 0.8f * s, 0.22f * s), Color(0.15f, 0.18f, 0.25f));
	if (p_item.variant == 3) {
		_cyl(p_root, "Helmet", Vector3(0, 1.95f * s, 0), 0.05f * s, 0.30f * s, 0.4f * s, Color(0.65f, 0.65f, 0.70f));
		_box(p_root, "Shield", Vector3(-0.62f * s, 1.0f * s, 0.1f * s), Vector3(0.06f, 0.7f * s, 0.5f * s), Color(0.85f, 0.15f, 0.15f));
	} else if (p_item.variant == 4) {
		_box(p_root, "Antenna", Vector3(0, 2.1f * s, 0), Vector3(0.05f * s, 0.35f * s, 0.05f * s), Color(0.20f, 0.70f, 0.85f));
		_sphere(p_root, "AntennaTip", Vector3(0, 2.3f * s, 0), 0.08f * s, Color(0.85f, 0.15f, 0.15f));
	} else if (p_item.variant == 5) {
		_sphere(p_root, "EyeL", Vector3(-0.1f * s, 1.78f * s, 0.24f * s), 0.06f * s, Color(0.05f, 0.05f, 0.05f));
		_sphere(p_root, "EyeR", Vector3(0.1f * s, 1.78f * s, 0.24f * s), 0.06f * s, Color(0.05f, 0.05f, 0.05f));
	}
}

static void _build_animal(Node3D *p_root, const ModelStoreCatalog::Item &p_item) {
	const float s = p_item.size;
	_box(p_root, "Body", Vector3(0, 0.5f * s, 0), Vector3(0.45f * s, 0.4f * s, 0.9f * s), p_item.color_a);
	_box(p_root, "Head", Vector3(0, 0.65f * s, 0.55f * s), Vector3(0.35f * s, 0.35f * s, 0.35f * s), p_item.color_a);
	_box(p_root, "EarL", Vector3(-0.12f * s, 0.88f * s, 0.55f * s), Vector3(0.08f * s, 0.15f * s, 0.05f), p_item.color_b);
	_box(p_root, "EarR", Vector3(0.12f * s, 0.88f * s, 0.55f * s), Vector3(0.08f * s, 0.15f * s, 0.05f), p_item.color_b);
	for (int i = 0; i < 4; i++) {
		float x = (i % 2 == 0 ? -0.16f : 0.16f) * s;
		float z = (i < 2 ? 0.3f : -0.3f) * s;
		_cyl(p_root, "Leg" + itos(i), Vector3(x, 0.15f * s, z), 0.06f * s, 0.06f * s, 0.3f * s, p_item.color_b);
	}
	_box(p_root, "Tail", Vector3(0, 0.6f * s, -0.55f * s), Vector3(0.08f * s, 0.08f * s, 0.3f * s), p_item.color_b);
}

static void _build_furniture(Node3D *p_root, const ModelStoreCatalog::Item &p_item) {
	const float s = p_item.size;
	if (p_item.variant == 0) {
		_box(p_root, "Seat", Vector3(0, 0.45f * s, 0), Vector3(0.5f * s, 0.08f * s, 0.5f * s), p_item.color_a);
		_box(p_root, "Back", Vector3(0, 0.75f * s, -0.22f * s), Vector3(0.5f * s, 0.6f * s, 0.08f * s), p_item.color_a);
		for (int i = 0; i < 4; i++) {
			float x = (i % 2 == 0 ? -0.2f : 0.2f) * s;
			float z = (i < 2 ? 0.2f : -0.2f) * s;
			_box(p_root, "Leg" + itos(i), Vector3(x, 0.2f * s, z), Vector3(0.07f * s, 0.4f * s, 0.07f * s), p_item.color_b);
		}
	} else if (p_item.variant == 1) {
		_box(p_root, "Top", Vector3(0, 0.7f * s, 0), Vector3(1.2f * s, 0.08f * s, 0.8f * s), p_item.color_a);
		for (int i = 0; i < 4; i++) {
			float x = (i % 2 == 0 ? -0.5f : 0.5f) * s;
			float z = (i < 2 ? 0.3f : -0.3f) * s;
			_box(p_root, "Leg" + itos(i), Vector3(x, 0.35f * s, z), Vector3(0.08f * s, 0.7f * s, 0.08f * s), p_item.color_b);
		}
	} else {
		_box(p_root, "Seat", Vector3(0, 0.45f * s, 0), Vector3(1.6f * s, 0.1f * s, 0.5f * s), p_item.color_a);
		_box(p_root, "Back", Vector3(0, 0.8f * s, -0.22f * s), Vector3(1.6f * s, 0.6f * s, 0.08f * s), p_item.color_a);
		_box(p_root, "LegL", Vector3(-0.7f * s, 0.2f * s, 0), Vector3(0.1f * s, 0.4f * s, 0.45f * s), p_item.color_b);
		_box(p_root, "LegR", Vector3(0.7f * s, 0.2f * s, 0), Vector3(0.1f * s, 0.4f * s, 0.45f * s), p_item.color_b);
	}
}

static void _build_prop(Node3D *p_root, const ModelStoreCatalog::Item &p_item) {
	const float s = p_item.size;
	if (p_item.variant == 1) {
		// Barrel.
		_cyl(p_root, "Barrel", Vector3(0, 0.4f * s, 0), 0.32f * s, 0.32f * s, 0.8f * s, p_item.color_a, 16);
		_cyl(p_root, "Band", Vector3(0, 0.55f * s, 0), 0.34f * s, 0.34f * s, 0.06f, Color(0.2f, 0.2f, 0.22f), 16);
	} else {
		// Crate.
		_box(p_root, "Crate", Vector3(0, 0.35f * s, 0), Vector3(0.7f * s, 0.7f * s, 0.7f * s), p_item.color_a);
		_box(p_root, "EdgeH", Vector3(0, 0.35f * s, 0.36f * s), Vector3(0.7f * s, 0.08f * s, 0.03f), p_item.color_b);
		_box(p_root, "EdgeV", Vector3(0, 0.35f * s, 0.36f * s), Vector3(0.08f * s, 0.7f * s, 0.03f), p_item.color_b);
	}
}

static void _build_fence(Node3D *p_root, const ModelStoreCatalog::Item &p_item) {
	const float s = p_item.size;
	for (int i = -1; i <= 1; i++) {
		_box(p_root, "Post" + itos(i + 1), Vector3(i * 0.9f * s, 0.5f * s, 0), Vector3(0.12f * s, 1.0f * s, 0.12f * s), p_item.color_a);
	}
	_box(p_root, "RailTop", Vector3(0, 0.8f * s, 0), Vector3(2.0f * s, 0.1f * s, 0.06f), p_item.color_b);
	_box(p_root, "RailBottom", Vector3(0, 0.35f * s, 0), Vector3(2.0f * s, 0.1f * s, 0.06f), p_item.color_b);
}

static void _build_chest(Node3D *p_root, const ModelStoreCatalog::Item &p_item) {
	const float s = p_item.size;
	_box(p_root, "Base", Vector3(0, 0.3f * s, 0), Vector3(0.9f * s, 0.6f * s, 0.6f * s), p_item.color_a);
	MeshInstance3D *lid = _cyl(p_root, "Lid", Vector3(0, 0.6f * s, 0), 0.3f * s, 0.3f * s, 0.9f * s, p_item.color_a, 12);
	lid->set_rotation(Vector3(0, 0, Math::deg_to_rad(90.0f)));
	_box(p_root, "Trim", Vector3(0, 0.6f * s, 0.31f * s), Vector3(0.9f * s, 0.06f, 0.03f), p_item.color_b);
	_box(p_root, "Lock", Vector3(0, 0.5f * s, 0.32f * s), Vector3(0.15f * s, 0.2f * s, 0.05f), p_item.color_b);
}

static void _build_fountain(Node3D *p_root, const ModelStoreCatalog::Item &p_item) {
	const float s = p_item.size;
	_cyl(p_root, "Basin", Vector3(0, 0.25f * s, 0), 1.0f * s, 1.1f * s, 0.5f * s, p_item.color_a, 20);
	_cyl(p_root, "Water", Vector3(0, 0.45f * s, 0), 0.9f * s, 0.9f * s, 0.06f, p_item.color_b, 20);
	_cyl(p_root, "Column", Vector3(0, 0.8f * s, 0), 0.15f * s, 0.2f * s, 0.8f * s, p_item.color_a, 12);
	_sphere(p_root, "TopWater", Vector3(0, 1.3f * s, 0), 0.2f * s, p_item.color_b);
}

static void _build_statue(Node3D *p_root, const ModelStoreCatalog::Item &p_item) {
	const float s = p_item.size;
	_box(p_root, "Pedestal", Vector3(0, 0.3f * s, 0), Vector3(0.8f * s, 0.6f * s, 0.8f * s), p_item.color_b);
	_capsule(p_root, "Figure", Vector3(0, 1.1f * s, 0), 0.25f * s, 0.9f * s, p_item.color_a);
	_sphere(p_root, "Head", Vector3(0, 1.75f * s, 0), 0.22f * s, p_item.color_a);
}

Node3D *ModelStoreCatalog::build_model(const Item &p_item) {
	if (p_item.id.is_empty()) {
		return nullptr;
	}
	Node3D *root = memnew(Node3D);
	root->set_name(p_item.id);

	switch (p_item.kind) {
		case KIND_TREE:
			_build_tree(root, p_item);
			break;
		case KIND_BUSH:
			_build_bush(root, p_item);
			break;
		case KIND_FLOWER:
			_build_flower(root, p_item);
			break;
		case KIND_GROUND:
			_build_ground(root, p_item);
			break;
		case KIND_HILL:
			_build_hill(root, p_item);
			break;
		case KIND_ROCK:
			_build_rock(root, p_item);
			break;
		case KIND_ROAD:
			_build_road(root, p_item);
			break;
		case KIND_BRIDGE:
			_build_bridge(root, p_item);
			break;
		case KIND_SIGN:
			_build_sign(root, p_item);
			break;
		case KIND_LAMP:
			_build_lamp(root, p_item);
			break;
		case KIND_CAR:
			_build_car(root, p_item);
			break;
		case KIND_TRUCK:
			_build_truck(root, p_item);
			break;
		case KIND_BIKE:
			_build_bike(root, p_item);
			break;
		case KIND_BOAT:
			_build_boat(root, p_item);
			break;
		case KIND_PLANE:
			_build_plane(root, p_item);
			break;
		case KIND_HOUSE:
			_build_house(root, p_item);
			break;
		case KIND_TOWER:
			_build_tower(root, p_item);
			break;
		case KIND_WINDMILL:
			_build_windmill(root, p_item);
			break;
		case KIND_PYRAMID:
			_build_pyramid(root, p_item);
			break;
		case KIND_CHARACTER:
			_build_character(root, p_item);
			break;
		case KIND_ANIMAL:
			_build_animal(root, p_item);
			break;
		case KIND_FURNITURE:
			_build_furniture(root, p_item);
			break;
		case KIND_PROP:
			_build_prop(root, p_item);
			break;
		case KIND_FENCE:
			_build_fence(root, p_item);
			break;
		case KIND_CHEST:
			_build_chest(root, p_item);
			break;
		case KIND_FOUNTAIN:
			_build_fountain(root, p_item);
			break;
		case KIND_STATUE:
			_build_statue(root, p_item);
			break;
		default:
			break;
	}

	if (p_item.has_collision) {
		StaticBody3D *body = memnew(StaticBody3D);
		body->set_name("Collision");
		Ref<BoxShape3D> shape;
		shape.instantiate();
		if (p_item.kind == KIND_GROUND || p_item.kind == KIND_ROAD) {
			shape->set_size(Vector3(2.0f * p_item.size, 0.4f, 2.0f * p_item.size));
			CollisionShape3D *cs = memnew(CollisionShape3D);
			cs->set_shape(shape);
			cs->set_position(Vector3(0, -0.2f, 0));
			body->add_child(cs);
		} else {
			float h = p_item.size;
			if (p_item.kind == KIND_TOWER) {
				h = (p_item.variant == 1 ? 4.5f : 3.0f) * p_item.size;
			}
			shape->set_size(Vector3(p_item.size, h, p_item.size));
			CollisionShape3D *cs = memnew(CollisionShape3D);
			cs->set_shape(shape);
			cs->set_position(Vector3(0, h * 0.5f, 0));
			body->add_child(cs);
		}
		root->add_child(body);
	}

	return root;
}

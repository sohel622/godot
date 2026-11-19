#include "ai_prompt_dialog.h"

#include "core/input/input_enums.h"
#include "core/input/input_event.h"
#include "core/io/dir_access.h"
#include "core/io/file_access.h"
#include "core/io/json.h"
#include "core/io/resource_loader.h"
#include "core/math/color.h"
#include "core/object/callable_mp.h"
#include "core/object/script_language.h"
#include "core/string/ustring.h"
#include "scene/3d/camera_3d.h"
#include "scene/3d/light_3d.h"
#include "scene/3d/mesh_instance_3d.h"
#include "scene/3d/node_3d.h"
#include "scene/main/node.h"
#include "scene/resources/3d/primitive_meshes.h"
#include "scene/resources/material.h"
#include "scene/resources/packed_scene.h"

#ifdef TOOLS_ENABLED
#include "editor/editor_interface.h"
#include "editor/editor_undo_redo_manager.h"
#include "editor/file_system/editor_file_system.h"
#include "editor/settings/editor_settings.h"
#endif

static const int AI_DIALOG_MIN_W = 550;
static const int AI_DIALOG_MIN_H = 420;
static const int AI_DIALOG_MAX_W = 1400;
static const int AI_DIALOG_MAX_H = 1000;
static const int AI_HANDLE_SIZE = 16;

// ---------------------------------------------------------------------------
// Settings (EditorSettings). The API key is never written in code; it only
// lives in the user's Editor Settings file. The old "gemini_api_key" key is
// still read as a fallback so a key entered before this change keeps working.
// ---------------------------------------------------------------------------

String AIPromptDialog::_settings_get(const String &p_name, const String &p_default) const {
#ifdef TOOLS_ENABLED
	if (EditorSettings::get_singleton() == nullptr) {
		return p_default;
	}
	Variant value = EditorSettings::get_singleton()->get_setting(p_name);
	if (value.get_type() == Variant::STRING) {
		return String(value);
	}
	return p_default;
#else
	return p_default;
#endif
}

void AIPromptDialog::_settings_set(const String &p_name, const String &p_value) {
#ifdef TOOLS_ENABLED
	if (EditorSettings::get_singleton() == nullptr) {
		return;
	}
	EditorSettings::get_singleton()->set_setting(p_name, p_value);
	EditorSettings::get_singleton()->save();
#endif
}

String AIPromptDialog::_provider_default_model(const String &p_provider) const {
	if (p_provider == "openai") {
		return "gpt-4o-mini";
	}
	if (p_provider == "anthropic") {
		return "claude-sonnet-4-5";
	}
	if (p_provider == "groq") {
		return "llama-3.3-70b-versatile";
	}
	if (p_provider == "custom") {
		return "";
	}
	return "gemini-1.5-flash";
}

String AIPromptDialog::_provider_default_base_url(const String &p_provider) const {
	if (p_provider == "openai") {
		return "https://api.openai.com/v1";
	}
	if (p_provider == "anthropic") {
		return "https://api.anthropic.com/v1";
	}
	if (p_provider == "groq") {
		return "https://api.groq.com/openai/v1";
	}
	if (p_provider == "custom") {
		return "";
	}
	return "https://generativelanguage.googleapis.com";
}

void AIPromptDialog::_load_settings_into_panel() {
	current_provider = _settings_get("ai_builder/provider", "gemini");
	String api_key = _settings_get("ai_builder/api_key", "");
	if (api_key.is_empty()) {
		api_key = _settings_get("ai_builder/gemini_api_key", "");
	}
	current_model = _settings_get("ai_builder/model", "");
	if (current_model.is_empty()) {
		current_model = _provider_default_model(current_provider);
	}
	current_base_url = _settings_get("ai_builder/base_url", "");
	if (current_base_url.is_empty()) {
		current_base_url = _provider_default_base_url(current_provider);
	}

	int provider_index = PROVIDER_GEMINI;
	if (current_provider == "openai") {
		provider_index = PROVIDER_OPENAI;
	} else if (current_provider == "anthropic") {
		provider_index = PROVIDER_ANTHROPIC;
	} else if (current_provider == "groq") {
		provider_index = PROVIDER_GROQ;
	} else if (current_provider == "custom") {
		provider_index = PROVIDER_CUSTOM;
	}
	provider_option->select(provider_index);
	api_key_edit->set_text(api_key);
	model_edit->set_text(current_model);
	base_url_edit->set_text(current_base_url);
}

// ---------------------------------------------------------------------------
// Settings panel callbacks.
// ---------------------------------------------------------------------------

void AIPromptDialog::_on_settings_pressed() {
	settings_panel->set_visible(!settings_panel->is_visible());
}

void AIPromptDialog::_on_provider_selected(int p_index) {
	static const char *provider_names[5] = { "gemini", "openai", "anthropic", "groq", "custom" };
	if (p_index < 0 || p_index > PROVIDER_CUSTOM) {
		return;
	}
	current_provider = String(provider_names[p_index]);
	_settings_set("ai_builder/provider", current_provider);
	// Switching provider resets the model and endpoint to that provider's
	// defaults. The user can edit both fields afterwards.
	current_model = _provider_default_model(current_provider);
	current_base_url = _provider_default_base_url(current_provider);
	model_edit->set_text(current_model);
	base_url_edit->set_text(current_base_url);
	_settings_set("ai_builder/model", current_model);
	_settings_set("ai_builder/base_url", current_base_url);
	settings_status->set_text("Provider changed. Model and endpoint set to defaults - you can edit them. Settings saved.");
}

void AIPromptDialog::_on_api_key_changed(const String &p_text) {
	_settings_set("ai_builder/api_key", p_text);
	settings_status->set_text("Settings saved in Editor Settings.");
}

void AIPromptDialog::_on_model_changed(const String &p_text) {
	current_model = p_text;
	_settings_set("ai_builder/model", p_text);
	settings_status->set_text("Settings saved in Editor Settings.");
}

void AIPromptDialog::_on_base_url_changed(const String &p_text) {
	current_base_url = p_text;
	_settings_set("ai_builder/base_url", p_text);
	settings_status->set_text("Settings saved in Editor Settings.");
}

// ---------------------------------------------------------------------------
// Resize handles (mouse + touch drag on the bottom corners and edges).
// ---------------------------------------------------------------------------

Control *AIPromptDialog::_make_resize_handle(int p_handle, Control::CursorShape p_cursor) {
	Control *handle = memnew(Control);
	handle->set_custom_minimum_size(Vector2i(AI_HANDLE_SIZE, AI_HANDLE_SIZE));
	handle->set_mouse_filter(Control::MOUSE_FILTER_STOP);
	handle->set_default_cursor_shape(p_cursor);
	handle->set_focus_mode(Control::FOCUS_NONE);
	switch (p_handle) {
		case HANDLE_BR:
			handle->set_anchors_preset(Control::PRESET_BOTTOM_RIGHT);
			handle->set_offset(SIDE_LEFT, -AI_HANDLE_SIZE);
			handle->set_offset(SIDE_TOP, -AI_HANDLE_SIZE);
			handle->set_offset(SIDE_RIGHT, 0);
			handle->set_offset(SIDE_BOTTOM, 0);
			break;
		case HANDLE_BL:
			handle->set_anchors_preset(Control::PRESET_BOTTOM_LEFT);
			handle->set_offset(SIDE_LEFT, 0);
			handle->set_offset(SIDE_TOP, -AI_HANDLE_SIZE);
			handle->set_offset(SIDE_RIGHT, AI_HANDLE_SIZE);
			handle->set_offset(SIDE_BOTTOM, 0);
			break;
		case HANDLE_RIGHT:
			handle->set_anchors_preset(Control::PRESET_CENTER_RIGHT);
			handle->set_offset(SIDE_LEFT, -AI_HANDLE_SIZE);
			handle->set_offset(SIDE_TOP, -AI_HANDLE_SIZE / 2);
			handle->set_offset(SIDE_RIGHT, 0);
			handle->set_offset(SIDE_BOTTOM, AI_HANDLE_SIZE / 2);
			break;
		case HANDLE_BOTTOM:
		default:
			handle->set_anchors_preset(Control::PRESET_CENTER_BOTTOM);
			handle->set_offset(SIDE_LEFT, -AI_HANDLE_SIZE / 2);
			handle->set_offset(SIDE_TOP, -AI_HANDLE_SIZE);
			handle->set_offset(SIDE_RIGHT, AI_HANDLE_SIZE / 2);
			handle->set_offset(SIDE_BOTTOM, 0);
			break;
	}
	handle->connect("gui_input", callable_mp(this, &AIPromptDialog::_on_resize_handle_gui_input).bind(p_handle));
	return handle;
}

void AIPromptDialog::_on_resize_handle_gui_input(const Ref<InputEvent> &p_event, int p_handle) {
	Ref<InputEventMouseButton> mouse_button = p_event;
	if (mouse_button.is_valid()) {
		if (mouse_button->get_button_index() == MouseButton::LEFT) {
			if (mouse_button->is_pressed()) {
				resizing = true;
				active_handle = p_handle;
			} else if (resizing && active_handle == p_handle) {
				resizing = false;
				active_handle = -1;
			}
			get_viewport()->set_input_as_handled();
		}
		return;
	}

	Ref<InputEventMouseMotion> mouse_motion = p_event;
	if (mouse_motion.is_valid()) {
		if (resizing && active_handle == p_handle && mouse_motion->get_button_mask().has_flag(MouseButtonMask::LEFT)) {
			_apply_resize_drag(mouse_motion->get_relative(), p_handle);
			get_viewport()->set_input_as_handled();
		}
		return;
	}

	Ref<InputEventScreenTouch> touch = p_event;
	if (touch.is_valid()) {
		if (touch->is_pressed()) {
			resizing = true;
			active_handle = p_handle;
		} else if (resizing && active_handle == p_handle) {
			resizing = false;
			active_handle = -1;
		}
		get_viewport()->set_input_as_handled();
		return;
	}

	Ref<InputEventScreenDrag> drag = p_event;
	if (drag.is_valid()) {
		if (resizing && active_handle == p_handle) {
			_apply_resize_drag(drag->get_relative(), p_handle);
			get_viewport()->set_input_as_handled();
		}
		return;
	}
}

void AIPromptDialog::_apply_resize_drag(const Vector2 &p_delta, int p_handle) {
	Size2i current_size = get_size();
	Point2i current_pos = get_position();
	Size2i next_size = current_size;
	Point2i next_pos = current_pos;

	switch (p_handle) {
		case HANDLE_BR:
			next_size = current_size + Vector2i((int)p_delta.x, (int)p_delta.y);
			break;
		case HANDLE_BL:
			next_size = current_size + Vector2i((int)-p_delta.x, (int)p_delta.y);
			break;
		case HANDLE_RIGHT:
			next_size = current_size + Vector2i((int)p_delta.x, 0);
			break;
		case HANDLE_BOTTOM:
		default:
			next_size = current_size + Vector2i(0, (int)p_delta.y);
			break;
	}

	if (next_size.x < AI_DIALOG_MIN_W) {
		next_size.x = AI_DIALOG_MIN_W;
	}
	if (next_size.x > AI_DIALOG_MAX_W) {
		next_size.x = AI_DIALOG_MAX_W;
	}
	if (next_size.y < AI_DIALOG_MIN_H) {
		next_size.y = AI_DIALOG_MIN_H;
	}
	if (next_size.y > AI_DIALOG_MAX_H) {
		next_size.y = AI_DIALOG_MAX_H;
	}
	// Bottom-left drag keeps the right edge fixed: move the window left/right
	// by exactly the amount the width actually changed.
	if (p_handle == HANDLE_BL) {
		next_pos.x = current_pos.x + (current_size.x - next_size.x);
	}
	set_size(next_size);
	set_position(next_pos);
}

// ---------------------------------------------------------------------------
// Chat helpers.
// ---------------------------------------------------------------------------

void AIPromptDialog::_append_chat(const String &p_text) {
	chat_history->set_text(chat_history->get_text() + "\n" + p_text);
}

// ---------------------------------------------------------------------------
// LLM request: one code path for Gemini, OpenAI, Anthropic, Groq and any
// OpenAI-compatible endpoint. All strings stay UTF-8 end to end: the prompt
// is already a Godot String (UTF-8 inside), JSON::stringify writes it out
// as UTF-8, and the reply bytes are decoded once with String::utf8().
// Never call String::utf8() on a String that is already decoded.
// ---------------------------------------------------------------------------

String AIPromptDialog::_build_system_prompt() const {
	return String::utf8("You are the Sohel Engine AI Builder inside the Godot editor. "
						 "Reply in the same language as the user (Bengali if the user writes Bengali, otherwise English). "
						 "Always answer with a fenced ```json block containing exactly this shape: "
						 "{\"reply\": \"short status text for the user\", \"actions\": [ ... ]}. "
						 "The \"reply\" text is shown in the chat. \"actions\" may be empty. "
						 "Only use these action types, with these fields: "
						 "{\"type\": \"new_scene\", \"name\": \"SceneName\"} - create a new empty 3D scene (only when no scene is open). "
						 "{\"type\": \"add_node\", \"node\": \"Node3D\", \"name\": \"Player\", \"parent\": \"\", \"position\": [0, 1, 0], \"rotation\": [0, 0, 0], \"scale\": [1, 1, 1]} - rotation is in degrees. "
						 "{\"type\": \"add_mesh\", \"mesh\": \"box\", \"name\": \"Ground\", \"size\": [4, 0.5, 4], \"position\": [0, 0, 0], \"color\": \"#888888\"} - mesh is one of box, sphere, capsule, cylinder, plane, prism. "
						 "{\"type\": \"add_light\", \"light\": \"directional\", \"name\": \"Sun\", \"rotation\": [-50, 30, 0], \"energy\": 1.2, \"range\": 10, \"angle\": 45, \"shadow\": true} - light is directional, omni or spot. "
						 "{\"type\": \"add_camera\", \"name\": \"Camera3D\", \"position\": [0, 3, 6], \"rotation\": [0, 0, 0], \"fov\": 70, \"current\": true}. "
						 "{\"type\": \"load_model\", \"path\": \"res://models/player.glb\", \"name\": \"PlayerModel\", \"position\": [0, 0, 0], \"scale\": [1, 1, 1]} - path must be an exact res:// path the user gave or that exists in the project. Never invent a model path. "
						 "{\"type\": \"make_script\", \"path\": \"res://ai_generated/move.gd\", \"code\": \"extends Node3D\\nfunc _process(delta): pass\", \"attach_to\": \"Player\"} - code must be complete, valid GDScript starting with extends. "
						 "{\"type\": \"attach_motion\", \"target\": \"Player\", \"mode\": \"move\", \"speed\": 3.0, \"amplitude\": 2.0, \"axis\": [1, 0, 0]} - mode is move, spin or bob. "
						 "{\"type\": \"save_scene\"}. "
						 "Keep scripts short and valid. If the user only asks a question, return an empty actions list. "
						 "Never put the JSON block inside any other text except the reply field.");
}

bool AIPromptDialog::_build_request(const String &p_prompt, String &r_url, Vector<String> &r_headers, String &r_body) const {
	String api_key = _settings_get("ai_builder/api_key", "");
	if (api_key.is_empty()) {
		api_key = _settings_get("ai_builder/gemini_api_key", "");
	}
	if (api_key.is_empty()) {
		return false;
	}
	String model = current_model.is_empty() ? _provider_default_model(current_provider) : current_model;
	String base_url = current_base_url.is_empty() ? _provider_default_base_url(current_provider) : current_base_url;
	String system_prompt = _build_system_prompt();

	r_headers.push_back("Content-Type: application/json");

	if (current_provider == "gemini") {
		r_url = base_url.rstrip("/") + "/v1beta/models/" + model + ":generateContent?key=" + api_key;
		Dictionary part;
		part["text"] = p_prompt;
		Array parts;
		parts.push_back(part);
		Dictionary content;
		content["parts"] = parts;
		Array contents;
		contents.push_back(content);
		Dictionary system_part;
		system_part["text"] = system_prompt;
		Array system_parts;
		system_parts.push_back(system_part);
		Dictionary system_instruction;
		system_instruction["parts"] = system_parts;
		Dictionary body;
		body["contents"] = contents;
		body["system_instruction"] = system_instruction;
		r_body = JSON::stringify(body);
		return true;
	}

	if (current_provider == "anthropic") {
		r_url = base_url.rstrip("/") + "/v1/messages";
		r_headers.push_back("x-api-key: " + api_key);
		r_headers.push_back("anthropic-version: 2023-06-01");
		Dictionary message;
		message["role"] = "user";
		message["content"] = p_prompt;
		Array messages;
		messages.push_back(message);
		Dictionary body;
		body["model"] = model;
		body["max_tokens"] = 4096;
		body["system"] = system_prompt;
		body["messages"] = messages;
		r_body = JSON::stringify(body);
		return true;
	}

	// OpenAI, Groq and any custom OpenAI-compatible endpoint share the
	// chat/completions format.
	r_url = base_url.rstrip("/") + "/chat/completions";
	r_headers.push_back("Authorization: Bearer " + api_key);
	Dictionary system_message;
	system_message["role"] = "system";
	system_message["content"] = system_prompt;
	Dictionary user_message;
	user_message["role"] = "user";
	user_message["content"] = p_prompt;
	Array messages;
	messages.push_back(system_message);
	messages.push_back(user_message);
	Dictionary body;
	body["model"] = model;
	body["messages"] = messages;
	r_body = JSON::stringify(body);
	return true;
}

String AIPromptDialog::_extract_reply_text(const Dictionary &p_response, const String &p_provider) const {
	if (p_provider == "gemini") {
		if (!p_response.has("candidates")) {
			return "";
		}
		Variant candidates_var = p_response["candidates"];
		if (candidates_var.get_type() != Variant::ARRAY) {
			return "";
		}
		Array candidates = candidates_var;
		if (candidates.is_empty()) {
			return "";
		}
		Variant cand_var = candidates[0];
		if (cand_var.get_type() != Variant::DICTIONARY) {
			return "";
		}
		Dictionary cand = cand_var;
		if (!cand.has("content")) {
			return "";
		}
		Variant content_var = cand["content"];
		if (content_var.get_type() != Variant::DICTIONARY) {
			return "";
		}
		Dictionary content = content_var;
		if (!content.has("parts")) {
			return "";
		}
		Variant parts_var = content["parts"];
		if (parts_var.get_type() != Variant::ARRAY) {
			return "";
		}
		Array parts = parts_var;
		if (parts.is_empty()) {
			return "";
		}
		Variant part_var = parts[0];
		if (part_var.get_type() != Variant::DICTIONARY) {
			return "";
		}
		Dictionary part = part_var;
		if (!part.has("text")) {
			return "";
		}
		Variant text_var = part["text"];
		if (text_var.get_type() != Variant::STRING) {
			return "";
		}
		return String(text_var);
	}

	if (p_provider == "anthropic") {
		if (!p_response.has("content")) {
			return "";
		}
		Variant content_var = p_response["content"];
		if (content_var.get_type() != Variant::ARRAY) {
			return "";
		}
		Array content = content_var;
		if (content.is_empty()) {
			return "";
		}
		Variant block_var = content[0];
		if (block_var.get_type() != Variant::DICTIONARY) {
			return "";
		}
		Dictionary block = block_var;
		if (!block.has("text")) {
			return "";
		}
		Variant text_var = block["text"];
		if (text_var.get_type() != Variant::STRING) {
			return "";
		}
		return String(text_var);
	}

	// OpenAI-compatible: choices[0].message.content
	if (!p_response.has("choices")) {
		return "";
	}
	Variant choices_var = p_response["choices"];
	if (choices_var.get_type() != Variant::ARRAY) {
		return "";
	}
	Array choices = choices_var;
	if (choices.is_empty()) {
		return "";
	}
	Variant choice_var = choices[0];
	if (choice_var.get_type() != Variant::DICTIONARY) {
		return "";
	}
	Dictionary choice = choice_var;
	if (!choice.has("message")) {
		return "";
	}
	Variant message_var = choice["message"];
	if (message_var.get_type() != Variant::DICTIONARY) {
		return "";
	}
	Dictionary message = message_var;
	if (!message.has("content")) {
		return "";
	}
	Variant content_var = message["content"];
	if (content_var.get_type() != Variant::STRING) {
		return "";
	}
	return String(content_var);
}

void AIPromptDialog::_on_send_pressed() {
	String prompt = prompt_input->get_text();
	if (prompt.is_empty()) {
		return;
	}

	_append_chat("User: " + prompt);
	prompt_input->clear();

	String full_prompt = prompt;
	if (!attached_path.is_empty()) {
		full_prompt += "\nAttached file: " + attached_path;
	}

	String url;
	Vector<String> headers;
	String body;
	if (!_build_request(full_prompt, url, headers, body)) {
		_append_chat("AI: API key দেওয়া হয়নি। উপরের Settings বাটন চাপুন, provider বেছে API key, model আর endpoint বসান, তারপর আবার চেষ্টা করুন।");
		return;
	}

	_append_chat("AI: প্রসেস করছে, অপেক্ষা করুন...");

	if (!http_client) {
		http_client = memnew(HTTPRequest);
		add_child(http_client);
		http_client->connect("request_completed", callable_mp(this, &AIPromptDialog::_on_request_completed));
	}

	Error err = http_client->request(url, headers, HTTPClient::METHOD_POST, body);
	if (err != OK) {
		_append_chat("AI: রিকোয়েস্ট পাঠাতে সমস্যা হয়েছে (error " + String::num_int64(err) + ")। ইন্টারনেট আর endpoint ঠিক আছে কিনা দেখুন।");
	}
}

void AIPromptDialog::_on_request_completed(int p_status, int p_code, const PackedStringArray &p_headers, const PackedByteArray &p_data) {
	if (p_status != HTTPRequest::RESULT_SUCCESS) {
		_append_chat("AI: নেটওয়ার্ক সমস্যা হয়েছে (status " + String::num_int64(p_status) + ")। ইন্টারনেট দেখে আবার চেষ্টা করুন।");
		return;
	}
	if (p_code < 200 || p_code >= 300) {
		String error_text = "";
		if (!p_data.is_empty()) {
			// Decode the response bytes exactly once as UTF-8.
			String response_text = String::utf8((const char *)p_data.ptr(), p_data.size());
			JSON json;
			if (json.parse(response_text) == OK) {
				Variant data = json.get_data();
				if (data.get_type() == Variant::DICTIONARY) {
					Dictionary dict = data;
					if (dict.has("error")) {
						Variant error_var = dict["error"];
						if (error_var.get_type() == Variant::DICTIONARY) {
							Dictionary error_dict = error_var;
							if (error_dict.has("message")) {
								Variant message_var = error_dict["message"];
								if (message_var.get_type() == Variant::STRING) {
									error_text = String(message_var);
								}
							}
						} else if (error_var.get_type() == Variant::STRING) {
							error_text = String(error_var);
						}
					}
				}
			}
		}
		if (error_text.is_empty()) {
			error_text = "HTTP " + String::num_int64(p_code);
		}
		_append_chat("AI: API থেকে উত্তর আসেনি (" + error_text + ")। Settings-এ API key, model আর endpoint ঠিক আছে কিনা দেখুন।");
		return;
	}
	if (p_data.is_empty()) {
		_append_chat("AI: API থেকে খালি উত্তর এসেছে। আবার চেষ্টা করুন।");
		return;
	}

	// Decode the response bytes exactly once as UTF-8. The old code used
	// String::parse_utf8(), which no longer exists in this Godot version.
	String response_text = String::utf8((const char *)p_data.ptr(), p_data.size());

	JSON json;
	Error err = json.parse(response_text);
	if (err != OK) {
		_append_chat("AI: উত্তরের JSON পড়া যায়নি। আবার চেষ্টা করুন।");
		return;
	}
	Variant data = json.get_data();
	if (data.get_type() != Variant::DICTIONARY) {
		_append_chat("AI: উত্তরের ধরন ঠিক নয়। আবার চেষ্টা করুন।");
		return;
	}
	Dictionary response = data;
	String reply_text = _extract_reply_text(response, current_provider);
	if (reply_text.is_empty()) {
		_append_chat("AI: উত্তর পেতে সমস্যা হয়েছে বা কোটা শেষ। Settings-এ provider আর model ঠিক আছে কিনা দেখুন।");
		return;
	}
	_handle_ai_reply(reply_text);
}

void AIPromptDialog::_handle_ai_reply(const String &p_text) {
	// Find the fenced ```json block, or fall back to the whole reply.
	String json_text;
	int fence = p_text.find("```json");
	if (fence >= 0) {
		int start = p_text.find("{", fence);
		int end = p_text.rfind("}");
		if (start >= 0 && end > start) {
			json_text = p_text.substr(start, end - start + 1);
		}
	} else {
		int start = p_text.find("{");
		int end = p_text.rfind("}");
		if (start >= 0 && end > start) {
			json_text = p_text.substr(start, end - start + 1);
		}
	}

	if (json_text.is_empty()) {
		_append_chat("AI:\n" + p_text);
		return;
	}

	JSON json;
	if (json.parse(json_text) != OK) {
		_append_chat("AI:\n" + p_text);
		return;
	}
	Variant data = json.get_data();
	if (data.get_type() != Variant::DICTIONARY) {
		_append_chat("AI:\n" + p_text);
		return;
	}
	Dictionary reply = data;

	if (reply.has("reply")) {
		Variant reply_var = reply["reply"];
		if (reply_var.get_type() == Variant::STRING) {
			_append_chat("AI:\n" + String(reply_var));
		} else {
			_append_chat("AI:\n" + p_text);
		}
	} else {
		_append_chat("AI:\n" + p_text);
	}

	if (reply.has("actions")) {
		Variant actions_var = reply["actions"];
		if (actions_var.get_type() == Variant::ARRAY) {
			_execute_actions(actions_var);
		}
	}
}

void AIPromptDialog::_on_attach_pressed() {
	if (!file_dialog) {
		file_dialog = memnew(FileDialog);
		file_dialog->set_file_mode(FileDialog::FILE_MODE_OPEN_FILE);
		file_dialog->set_access(FileDialog::ACCESS_FILESYSTEM);
		file_dialog->add_filter("*.png, *.jpg, *.glb, *.gltf ; Supported Assets");
		file_dialog->connect("file_selected", callable_mp(this, &AIPromptDialog::_on_file_selected));
		add_child(file_dialog);
	}
	file_dialog->popup_centered(Vector2i(600, 400));
}

void AIPromptDialog::_on_file_selected(const String &p_path) {
	attached_path = p_path;
	_append_chat("[অ্যাসেট সংযুক্ত হয়েছে]: " + p_path);
}

// ---------------------------------------------------------------------------
// Structured scene actions. Everything runs on the main thread, inside the
// editor, with null checks at every step so a bad AI answer can only produce
// a chat error message, never a crash or a frozen editor. Scene edits use
// the editor UndoRedoManager so Ctrl+Z works.
// ---------------------------------------------------------------------------

Node *AIPromptDialog::_edited_scene_root() const {
#ifdef TOOLS_ENABLED
	EditorInterface *editor = EditorInterface::get_singleton();
	if (editor == nullptr) {
		return nullptr;
	}
	return editor->get_edited_scene_root();
#else
	return nullptr;
#endif
}

void AIPromptDialog::_mark_scene_unsaved() {
#ifdef TOOLS_ENABLED
	EditorInterface *editor = EditorInterface::get_singleton();
	if (editor != nullptr) {
		editor->mark_scene_as_unsaved();
	}
#endif
}

Node *AIPromptDialog::_find_node_by_name(Node *p_from, const String &p_name) const {
	if (p_from == nullptr || p_name.is_empty()) {
		return nullptr;
	}
	if (p_name.contains("/")) {
		return p_from->get_node_or_null(NodePath(p_name));
	}
	if (p_from->get_name() == p_name) {
		return p_from;
	}
	for (int i = 0; i < p_from->get_child_count(); i++) {
		Node *found = _find_node_by_name(p_from->get_child(i), p_name);
		if (found != nullptr) {
			return found;
		}
	}
	return nullptr;
}

String AIPromptDialog::_unique_child_name(Node *p_parent, const String &p_base) const {
	if (p_parent == nullptr) {
		return p_base;
	}
	String candidate = p_base.is_empty() ? "Node" : p_base;
	if (p_parent->get_node_or_null(NodePath(candidate)) == nullptr) {
		return candidate;
	}
	for (int i = 2; i < 1000; i++) {
		candidate = p_base + "_" + String::num_int64(i);
		if (p_parent->get_node_or_null(NodePath(candidate)) == nullptr) {
			return candidate;
		}
	}
	return p_base + "_" + String::num_int64(1000);
}

Vector3 AIPromptDialog::_dict_vector3(const Dictionary &p_dict, const String &p_key, const Vector3 &p_default) const {
	if (!p_dict.has(p_key)) {
		return p_default;
	}
	Variant value = p_dict[p_key];
	if (value.get_type() != Variant::ARRAY) {
		return p_default;
	}
	Array arr = value;
	if (arr.size() < 3) {
		return p_default;
	}
	double x = p_default.x;
	double y = p_default.y;
	double z = p_default.z;
	if (arr[0].get_type() == Variant::FLOAT || arr[0].get_type() == Variant::INT) {
		x = double(arr[0]);
	}
	if (arr[1].get_type() == Variant::FLOAT || arr[1].get_type() == Variant::INT) {
		y = double(arr[1]);
	}
	if (arr[2].get_type() == Variant::FLOAT || arr[2].get_type() == Variant::INT) {
		z = double(arr[2]);
	}
	return Vector3((float)x, (float)y, (float)z);
}

double AIPromptDialog::_dict_double(const Dictionary &p_dict, const String &p_key, double p_default) const {
	if (!p_dict.has(p_key)) {
		return p_default;
	}
	Variant value = p_dict[p_key];
	if (value.get_type() == Variant::FLOAT || value.get_type() == Variant::INT) {
		return double(value);
	}
	return p_default;
}

bool AIPromptDialog::_dict_bool(const Dictionary &p_dict, const String &p_key, bool p_default) const {
	if (!p_dict.has(p_key)) {
		return p_default;
	}
	Variant value = p_dict[p_key];
	if (value.get_type() == Variant::BOOL) {
		return bool(value);
	}
	return p_default;
}

String AIPromptDialog::_dict_string(const Dictionary &p_dict, const String &p_key, const String &p_default) const {
	if (!p_dict.has(p_key)) {
		return p_default;
	}
	Variant value = p_dict[p_key];
	if (value.get_type() == Variant::STRING) {
		return String(value);
	}
	return p_default;
}

void AIPromptDialog::_add_node_with_undo(Node *p_parent, Node *p_node, Node *p_scene_root, const String &p_action_name) {
	if (p_parent == nullptr || p_node == nullptr) {
		return;
	}
#ifdef TOOLS_ENABLED
	EditorInterface *editor = EditorInterface::get_singleton();
	EditorUndoRedoManager *undo = (editor != nullptr) ? editor->get_editor_undo_redo() : nullptr;
	if (undo != nullptr && p_scene_root != nullptr) {
		undo->create_action(p_action_name);
		undo->add_do_method(p_parent, "add_child", p_node);
		undo->add_do_method(p_node, "set_owner", p_scene_root);
		undo->add_do_reference(p_node);
		undo->add_undo_method(p_parent, "remove_child", p_node);
		undo->commit_action();
		_mark_scene_unsaved();
		return;
	}
#endif
	p_parent->add_child(p_node);
	if (p_scene_root != nullptr) {
		p_node->set_owner(p_scene_root);
	}
	_mark_scene_unsaved();
}

String AIPromptDialog::_motion_script_code(const String &p_mode, double p_speed, double p_amplitude, const Vector3 &p_axis) const {
	String axis = "Vector3(" + String::num(p_axis.x) + ", " + String::num(p_axis.y) + ", " + String::num(p_axis.z) + ")";
	String body;
	if (p_mode == "spin") {
		body = "\trotate_y(delta * " + String::num(p_speed) + ")\n";
	} else if (p_mode == "bob") {
		body = "\t_t += delta * " + String::num(p_speed) + "\n";
		body += "\tposition = _base + Vector3.UP * sin(_t) * " + String::num(p_amplitude) + "\n";
	} else {
		// move: slide back and forth along the given axis.
		body = "\t_t += delta * " + String::num(p_speed) + "\n";
		body += "\tposition = _base + (" + axis + ") * sin(_t) * " + String::num(p_amplitude) + "\n";
	}
	String code = "extends Node3D\n";
	code += "var _t := 0.0\n";
	code += "var _base := Vector3.ZERO\n";
	code += "@export var speed := " + String::num(p_speed) + "\n";
	code += "@export var amplitude := " + String::num(p_amplitude) + "\n";
	code += "func _ready() -> void:\n";
	code += "\t_base = position\n";
	code += "func _process(delta: float) -> void:\n";
	code += body;
	return code;
}

AIPromptDialog::ActionResult AIPromptDialog::_write_script_file(const String &p_path, const String &p_code) {
	ActionResult result;
	if (p_path.is_empty() || !p_path.begins_with("res://")) {
		result.ok = false;
		result.message = "Script path must start with res:// : " + p_path;
		return result;
	}
	String base_dir = p_path.get_base_dir();
	if (!base_dir.is_empty()) {
		DirAccess::make_dir_recursive_absolute(base_dir);
	}
	Ref<FileAccess> file = FileAccess::open(p_path, FileAccess::WRITE);
	if (file.is_null()) {
		result.ok = false;
		result.message = "Could not write script file: " + p_path;
		return result;
	}
	file->store_string(p_code);
	file->close();
#ifdef TOOLS_ENABLED
	EditorFileSystem *fs = EditorFileSystem::get_singleton();
	if (fs != nullptr) {
		fs->update_file(p_path);
	}
#endif
	result.ok = true;
	result.message = "Script written: " + p_path;
	return result;
}

void AIPromptDialog::_execute_actions(const Array &p_actions) {
	for (int i = 0; i < p_actions.size(); i++) {
		Variant action_var = p_actions[i];
		if (action_var.get_type() != Variant::DICTIONARY) {
			_append_chat("✗ Action skipped: not an object.");
			continue;
		}
		Dictionary action = action_var;
		ActionResult result = _execute_action(action);
		if (result.ok) {
			_append_chat("✓ " + result.message);
		} else {
			_append_chat("✗ " + result.message);
		}
	}
}

AIPromptDialog::ActionResult AIPromptDialog::_execute_action(const Dictionary &p_action) {
	ActionResult result;
	String type = _dict_string(p_action, "type", "");
	if (type.is_empty()) {
		result.ok = false;
		result.message = "Action has no type.";
		return result;
	}

#ifdef TOOLS_ENABLED
	EditorInterface *editor = EditorInterface::get_singleton();
	if (editor == nullptr) {
		result.ok = false;
		result.message = "Editor is not available, action '" + type + "' was not run.";
		return result;
	}

	if (type == "new_scene") {
		Node *existing_root = _edited_scene_root();
		if (existing_root != nullptr) {
			result.ok = false;
			result.message = "A scene is already open. Close it first, then ask for a new scene.";
			return result;
		}
		String name = _dict_string(p_action, "name", "NewScene");
		Node3D *root = memnew(Node3D);
		root->set_name(name);
		editor->add_root_node(root);
		_mark_scene_unsaved();
		result.ok = true;
		result.message = "New scene created: " + name;
		return result;
	}

	if (type == "save_scene") {
		Error err = editor->save_scene();
		if (err == OK) {
			result.ok = true;
			result.message = "Scene saved.";
		} else {
			result.ok = false;
			result.message = "Could not save the scene (error " + String::num_int64(err) + ").";
		}
		return result;
	}

	Node *scene_root = _edited_scene_root();
	if (scene_root == nullptr) {
		result.ok = false;
		result.message = "No scene is open. Ask for a new scene first, or open a scene, then try again.";
		return result;
	}

	Node *parent = scene_root;
	String parent_name = _dict_string(p_action, "parent", "");
	if (!parent_name.is_empty()) {
		parent = _find_node_by_name(scene_root, parent_name);
		if (parent == nullptr) {
			result.ok = false;
			result.message = "Parent node not found: " + parent_name;
			return result;
		}
	}

	if (type == "add_node") {
		String node_type = _dict_string(p_action, "node", "Node3D");
		String name = _unique_child_name(parent, _dict_string(p_action, "name", "Node3D"));
		Node *node = nullptr;
		if (node_type == "Node") {
			node = memnew(Node);
		} else if (node_type == "Node3D") {
			node = memnew(Node3D);
		} else {
			result.ok = false;
			result.message = "Unsupported node type: " + node_type + " (use Node3D or Node).";
			return result;
		}
		node->set_name(name);
		Node3D *node_3d = Object::cast_to<Node3D>(node);
		if (node_3d != nullptr) {
			node_3d->set_position(_dict_vector3(p_action, "position", Vector3(0, 0, 0)));
			node_3d->set_rotation_degrees(_dict_vector3(p_action, "rotation", Vector3(0, 0, 0)));
			node_3d->set_scale(_dict_vector3(p_action, "scale", Vector3(1, 1, 1)));
		}
		_add_node_with_undo(parent, node, scene_root, "Add Node " + name);
		result.ok = true;
		result.message = "Node added: " + name;
		return result;
	}

	if (type == "add_mesh") {
		String mesh_type = _dict_string(p_action, "mesh", "box").to_lower();
		String name = _unique_child_name(parent, _dict_string(p_action, "name", "Mesh"));
		Ref<PrimitiveMesh> mesh;
		if (mesh_type == "box") {
			Ref<BoxMesh> box;
			box.instantiate();
			box->set_size(_dict_vector3(p_action, "size", Vector3(1, 1, 1)));
			mesh = box;
		} else if (mesh_type == "sphere") {
			Ref<SphereMesh> sphere;
			sphere.instantiate();
			sphere->set_radius((float)_dict_double(p_action, "radius", 0.5));
			sphere->set_height((float)_dict_double(p_action, "height", 1.0));
			mesh = sphere;
		} else if (mesh_type == "capsule") {
			Ref<CapsuleMesh> capsule;
			capsule.instantiate();
			capsule->set_radius((float)_dict_double(p_action, "radius", 0.5));
			capsule->set_height((float)_dict_double(p_action, "height", 1.0));
			mesh = capsule;
		} else if (mesh_type == "cylinder") {
			Ref<CylinderMesh> cylinder;
			cylinder.instantiate();
			cylinder->set_top_radius((float)_dict_double(p_action, "top_radius", _dict_double(p_action, "radius", 0.5)));
			cylinder->set_bottom_radius((float)_dict_double(p_action, "bottom_radius", _dict_double(p_action, "radius", 0.5)));
			cylinder->set_height((float)_dict_double(p_action, "height", 1.0));
			mesh = cylinder;
		} else if (mesh_type == "plane") {
			Ref<PlaneMesh> plane;
			plane.instantiate();
			Vector3 size = _dict_vector3(p_action, "size", Vector3(4, 0, 4));
			plane->set_size(Size2(size.x, size.z));
			mesh = plane;
		} else if (mesh_type == "prism") {
			Ref<PrismMesh> prism;
			prism.instantiate();
			prism->set_size(_dict_vector3(p_action, "size", Vector3(1, 1, 1)));
			mesh = prism;
		} else {
			result.ok = false;
			result.message = "Unsupported mesh type: " + mesh_type + " (use box, sphere, capsule, cylinder, plane or prism).";
			return result;
		}
		String color_text = _dict_string(p_action, "color", "");
		if (!color_text.is_empty() && color_text.begins_with("#") && (color_text.length() == 7 || color_text.length() == 9)) {
			Ref<StandardMaterial3D> material;
			material.instantiate();
			material->set_albedo(Color::html(color_text));
			mesh->set_material(material);
		}
		MeshInstance3D *instance = memnew(MeshInstance3D);
		instance->set_name(name);
		instance->set_mesh(mesh);
		instance->set_position(_dict_vector3(p_action, "position", Vector3(0, 0, 0)));
		instance->set_rotation_degrees(_dict_vector3(p_action, "rotation", Vector3(0, 0, 0)));
		instance->set_scale(_dict_vector3(p_action, "scale", Vector3(1, 1, 1)));
		_add_node_with_undo(parent, instance, scene_root, "Add Mesh " + name);
		result.ok = true;
		result.message = "Mesh added: " + name + " (" + mesh_type + ")";
		return result;
	}

	if (type == "add_light") {
		String light_type = _dict_string(p_action, "light", "directional").to_lower();
		String name = _unique_child_name(parent, _dict_string(p_action, "name", "Light"));
		Light3D *light = nullptr;
		if (light_type == "directional") {
			light = memnew(DirectionalLight3D);
		} else if (light_type == "omni") {
			light = memnew(OmniLight3D);
		} else if (light_type == "spot") {
			light = memnew(SpotLight3D);
		} else {
			result.ok = false;
			result.message = "Unsupported light type: " + light_type + " (use directional, omni or spot).";
			return result;
		}
		light->set_name(name);
		light->set_position(_dict_vector3(p_action, "position", Vector3(0, 0, 0)));
		light->set_rotation_degrees(_dict_vector3(p_action, "rotation", Vector3(0, 0, 0)));
		light->set_param(Light3D::PARAM_ENERGY, (float)_dict_double(p_action, "energy", 1.0));
		if (light_type == "omni" || light_type == "spot") {
			light->set_param(Light3D::PARAM_RANGE, (float)_dict_double(p_action, "range", 10.0));
		}
		if (light_type == "spot") {
			light->set_param(Light3D::PARAM_SPOT_ANGLE, (float)_dict_double(p_action, "angle", 45.0));
		}
		light->set_shadow(_dict_bool(p_action, "shadow", true));
		_add_node_with_undo(parent, light, scene_root, "Add Light " + name);
		result.ok = true;
		result.message = "Light added: " + name + " (" + light_type + ")";
		return result;
	}

	if (type == "add_camera") {
		String name = _unique_child_name(parent, _dict_string(p_action, "name", "Camera3D"));
		Camera3D *camera = memnew(Camera3D);
		camera->set_name(name);
		camera->set_position(_dict_vector3(p_action, "position", Vector3(0, 3, 6)));
		camera->set_rotation_degrees(_dict_vector3(p_action, "rotation", Vector3(0, 0, 0)));
		camera->set_fov((float)_dict_double(p_action, "fov", 70.0));
		camera->set_current(_dict_bool(p_action, "current", true));
		_add_node_with_undo(parent, camera, scene_root, "Add Camera " + name);
		result.ok = true;
		result.message = "Camera added: " + name;
		return result;
	}

	if (type == "load_model") {
		String path = _dict_string(p_action, "path", "");
		if (path.is_empty() || !path.begins_with("res://")) {
			result.ok = false;
			result.message = "Model path must start with res:// : " + path;
			return result;
		}
		if (!ResourceLoader::exists(path)) {
			result.ok = false;
			result.message = "Model not found in the project: " + path + ". Import the model first, then give its exact res:// path.";
			return result;
		}
		Ref<PackedScene> packed = ResourceLoader::load(path, "PackedScene");
		if (packed.is_null()) {
			result.ok = false;
			result.message = "Could not load model as a scene: " + path;
			return result;
		}
		Node *instance = packed->instantiate();
		if (instance == nullptr) {
			result.ok = false;
			result.message = "Could not instantiate model: " + path;
			return result;
		}
		String name = _unique_child_name(parent, _dict_string(p_action, "name", "Model"));
		instance->set_name(name);
		Node3D *instance_3d = Object::cast_to<Node3D>(instance);
		if (instance_3d != nullptr) {
			instance_3d->set_position(_dict_vector3(p_action, "position", Vector3(0, 0, 0)));
			instance_3d->set_rotation_degrees(_dict_vector3(p_action, "rotation", Vector3(0, 0, 0)));
			instance_3d->set_scale(_dict_vector3(p_action, "scale", Vector3(1, 1, 1)));
		}
		_add_node_with_undo(parent, instance, scene_root, "Add Model " + name);
		result.ok = true;
		result.message = "Model added: " + name + " (" + path + ")";
		return result;
	}

	if (type == "make_script") {
		String path = _dict_string(p_action, "path", "");
		String code = _dict_string(p_action, "code", "");
		if (path.is_empty()) {
			path = "res://ai_generated/script.gd";
		}
		if (code.is_empty()) {
			result.ok = false;
			result.message = "Script action has no code.";
			return result;
		}
		String trimmed = code.strip_edges();
		if (!trimmed.begins_with("extends")) {
			code = "extends Node3D\n" + code;
		}
		ActionResult written = _write_script_file(path, code);
		if (!written.ok) {
			return written;
		}
		String attach_to = _dict_string(p_action, "attach_to", "");
		if (!attach_to.is_empty()) {
			Node *target = _find_node_by_name(scene_root, attach_to);
			if (target == nullptr) {
				result.ok = false;
				result.message = "Script written to " + path + ", but target node not found: " + attach_to;
				return result;
			}
			Ref<Script> script = ResourceLoader::load(path, "Script");
			if (script.is_null()) {
				result.ok = false;
				result.message = "Script written to " + path + ", but it could not be loaded (check the GDScript for errors).";
				return result;
			}
			target->set_script(script);
			_mark_scene_unsaved();
			result.ok = true;
			result.message = "Script written and attached: " + path + " -> " + attach_to;
			return result;
		}
		result.ok = true;
		result.message = "Script written: " + path;
		return result;
	}

	if (type == "attach_motion") {
		String target_name = _dict_string(p_action, "target", "");
		if (target_name.is_empty()) {
			result.ok = false;
			result.message = "Motion action needs a target node name.";
			return result;
		}
		Node *target = _find_node_by_name(scene_root, target_name);
		if (target == nullptr) {
			result.ok = false;
			result.message = "Motion target not found: " + target_name;
			return result;
		}
		String mode = _dict_string(p_action, "mode", "move").to_lower();
		if (mode != "move" && mode != "spin" && mode != "bob") {
			mode = "move";
		}
		double speed = _dict_double(p_action, "speed", 3.0);
		double amplitude = _dict_double(p_action, "amplitude", 2.0);
		Vector3 axis = _dict_vector3(p_action, "axis", Vector3(1, 0, 0));
		String safe_name;
		for (int i = 0; i < target_name.length(); i++) {
			char32_t c = target_name[i];
			if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_') {
				safe_name += String::chr(c);
			} else {
				safe_name += "_";
			}
		}
		String path = "res://ai_generated/" + safe_name + "_motion.gd";
		String code = _motion_script_code(mode, speed, amplitude, axis);
		ActionResult written = _write_script_file(path, code);
		if (!written.ok) {
			return written;
		}
		Ref<Script> script = ResourceLoader::load(path, "Script");
		if (script.is_null()) {
			result.ok = false;
			result.message = "Motion script written to " + path + ", but it could not be loaded.";
			return result;
		}
		target->set_script(script);
		_mark_scene_unsaved();
		result.ok = true;
		result.message = "Motion attached: " + target_name + " (" + mode + ", speed " + String::num(speed) + ")";
		return result;
	}

	result.ok = false;
	result.message = "Unknown action type: " + type;
	return result;
#else
	result.ok = false;
	result.message = "Scene actions need the Godot editor, action '" + type + "' was not run.";
	return result;
#endif
}

void AIPromptDialog::_bind_methods() {}

AIPromptDialog::AIPromptDialog() {
	set_title("Sohel Engine - AI Builder");
	set_min_size(Vector2i(AI_DIALOG_MIN_W, AI_DIALOG_MIN_H));
	set_max_size(Vector2i(AI_DIALOG_MAX_W, AI_DIALOG_MAX_H));
	// Let the OS window manager resize the dialog too, not just the handles.
	set_flag(FLAG_RESIZE_DISABLED, false);

	VBoxContainer *main_vbox = memnew(VBoxContainer);
	add_child(main_vbox);

	// Top bar: title on the left, Settings button on the right.
	HBoxContainer *top_bar = memnew(HBoxContainer);
	main_vbox->add_child(top_bar);

	Label *title_label = memnew(Label);
	title_label->set_text("AI Builder");
	top_bar->add_child(title_label);

	Control *top_spacer = memnew(Control);
	top_spacer->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	top_bar->add_child(top_spacer);

	settings_btn = memnew(Button);
	settings_btn->set_text("Settings");
	settings_btn->set_tooltip_text("Choose the AI provider and set the API key, model and endpoint.");
	settings_btn->connect("pressed", callable_mp(this, &AIPromptDialog::_on_settings_pressed));
	top_bar->add_child(settings_btn);

	// Settings panel (hidden until the Settings button is pressed).
	settings_panel = memnew(PanelContainer);
	settings_panel->set_visible(false);
	main_vbox->add_child(settings_panel);

	VBoxContainer *settings_vbox = memnew(VBoxContainer);
	settings_panel->add_child(settings_vbox);

	GridContainer *settings_grid = memnew(GridContainer);
	settings_grid->set_columns(2);
	settings_vbox->add_child(settings_grid);

	Label *provider_label = memnew(Label);
	provider_label->set_text("Provider:");
	settings_grid->add_child(provider_label);

	provider_option = memnew(OptionButton);
	provider_option->add_item("Gemini", PROVIDER_GEMINI);
	provider_option->add_item("OpenAI", PROVIDER_OPENAI);
	provider_option->add_item("Anthropic", PROVIDER_ANTHROPIC);
	provider_option->add_item("Groq", PROVIDER_GROQ);
	provider_option->add_item("Custom (OpenAI-compatible)", PROVIDER_CUSTOM);
	provider_option->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	provider_option->connect("item_selected", callable_mp(this, &AIPromptDialog::_on_provider_selected));
	settings_grid->add_child(provider_option);

	Label *api_key_label = memnew(Label);
	api_key_label->set_text("API Key:");
	settings_grid->add_child(api_key_label);

	api_key_edit = memnew(LineEdit);
	api_key_edit->set_secret(true);
	api_key_edit->set_placeholder("Paste your API key here");
	api_key_edit->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	api_key_edit->connect("text_changed", callable_mp(this, &AIPromptDialog::_on_api_key_changed));
	settings_grid->add_child(api_key_edit);

	Label *model_label = memnew(Label);
	model_label->set_text("Model:");
	settings_grid->add_child(model_label);

	model_edit = memnew(LineEdit);
	model_edit->set_placeholder("e.g. gemini-1.5-flash");
	model_edit->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	model_edit->connect("text_changed", callable_mp(this, &AIPromptDialog::_on_model_changed));
	settings_grid->add_child(model_edit);

	Label *base_url_label = memnew(Label);
	base_url_label->set_text("Endpoint:");
	settings_grid->add_child(base_url_label);

	base_url_edit = memnew(LineEdit);
	base_url_edit->set_placeholder("Leave empty for the provider default");
	base_url_edit->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	base_url_edit->connect("text_changed", callable_mp(this, &AIPromptDialog::_on_base_url_changed));
	settings_grid->add_child(base_url_edit);

	settings_status = memnew(Label);
	settings_status->set_text("Settings are saved in Editor Settings. The API key is never written in code.");
	settings_status->set_autowrap_mode(TextServer::AUTOWRAP_WORD_SMART);
	settings_vbox->add_child(settings_status);

	_load_settings_into_panel();

	chat_history = memnew(TextEdit);
	chat_history->set_custom_minimum_size(Vector2i(500, 260));
	chat_history->set_editable(false);
	main_vbox->add_child(chat_history);

	HBoxContainer *input_bar = memnew(HBoxContainer);
	main_vbox->add_child(input_bar);

	attach_btn = memnew(Button);
	attach_btn->set_text("+");
	attach_btn->set_tooltip_text("ছবি বা ৩ডি স্ট্রাকচার ফাইল যোগ করুন");
	attach_btn->connect("pressed", callable_mp(this, &AIPromptDialog::_on_attach_pressed));
	input_bar->add_child(attach_btn);

	prompt_input = memnew(LineEdit);
	prompt_input->set_placeholder("গেম তৈরির প্রম্পট লিখুন...");
	prompt_input->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	input_bar->add_child(prompt_input);

	send_btn = memnew(Button);
	send_btn->set_text("Generate");
	send_btn->connect("pressed", callable_mp(this, &AIPromptDialog::_on_send_pressed));
	input_bar->add_child(send_btn);

	// Drag-to-resize handles. Added last so they sit on top of the content.
	// Native OS resize is also enabled above (FLAG_RESIZE_DISABLED cleared).
	resize_handles[HANDLE_BR] = _make_resize_handle(HANDLE_BR, Control::CURSOR_FDIAGSIZE);
	add_child(resize_handles[HANDLE_BR]);
	resize_handles[HANDLE_BL] = _make_resize_handle(HANDLE_BL, Control::CURSOR_BDIAGSIZE);
	add_child(resize_handles[HANDLE_BL]);
	resize_handles[HANDLE_RIGHT] = _make_resize_handle(HANDLE_RIGHT, Control::CURSOR_HSIZE);
	add_child(resize_handles[HANDLE_RIGHT]);
	resize_handles[HANDLE_BOTTOM] = _make_resize_handle(HANDLE_BOTTOM, Control::CURSOR_VSIZE);
	add_child(resize_handles[HANDLE_BOTTOM]);
}

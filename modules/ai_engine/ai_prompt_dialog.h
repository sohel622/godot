#pragma once

#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/control.h"
#include "scene/gui/dialogs.h"
#include "scene/gui/file_dialog.h"
#include "scene/gui/grid_container.h"
#include "scene/gui/label.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/option_button.h"
#include "scene/gui/panel_container.h"
#include "scene/gui/text_edit.h"
#include "scene/main/http_request.h"

class AIPromptDialog : public AcceptDialog {
	GDCLASS(AIPromptDialog, AcceptDialog);

public:
	enum Provider {
		PROVIDER_GEMINI = 0,
		PROVIDER_OPENAI,
		PROVIDER_ANTHROPIC,
		PROVIDER_GROQ,
		PROVIDER_CUSTOM,
	};

private:
	// Chat UI.
	TextEdit *chat_history = nullptr;
	LineEdit *prompt_input = nullptr;
	Button *attach_btn = nullptr;
	Button *send_btn = nullptr;
	FileDialog *file_dialog = nullptr;
	HTTPRequest *http_client = nullptr;
	String attached_path;

	// Settings panel UI (top area of the dialog).
	Button *settings_btn = nullptr;
	PanelContainer *settings_panel = nullptr;
	OptionButton *provider_option = nullptr;
	LineEdit *api_key_edit = nullptr;
	LineEdit *model_edit = nullptr;
	LineEdit *base_url_edit = nullptr;
	Label *settings_status = nullptr;

	// Resize handles (bottom corners + bottom/right edges).
	enum ResizeHandle {
		HANDLE_BR = 0,
		HANDLE_BL,
		HANDLE_RIGHT,
		HANDLE_BOTTOM,
	};
	Control *resize_handles[4] = { nullptr, nullptr, nullptr, nullptr };
	bool resizing = false;
	int active_handle = -1;

	// Current provider configuration (loaded from EditorSettings).
	String current_provider;
	String current_model;
	String current_base_url;

	// EditorSettings helpers. The key is never written in code; it only
	// lives in the user's Editor Settings file.
	String _settings_get(const String &p_name, const String &p_default) const;
	void _settings_set(const String &p_name, const String &p_value);
	String _provider_default_model(const String &p_provider) const;
	String _provider_default_base_url(const String &p_provider) const;
	void _load_settings_into_panel();

	// Settings panel callbacks.
	void _on_settings_pressed();
	void _on_provider_selected(int p_index);
	void _on_api_key_changed(const String &p_text);
	void _on_model_changed(const String &p_text);
	void _on_base_url_changed(const String &p_text);
	Control *_make_resize_handle(int p_handle, Control::CursorShape p_cursor);

	// Chat / LLM request.
	void _on_send_pressed();
	void _on_attach_pressed();
	void _on_file_selected(const String &p_path);
	void _on_request_completed(int p_status, int p_code, const PackedStringArray &p_headers, const PackedByteArray &p_data);
	String _build_system_prompt() const;
	bool _build_request(const String &p_prompt, String &r_url, Vector<String> &r_headers, String &r_body) const;
	String _extract_reply_text(const Dictionary &p_response, const String &p_provider) const;
	void _handle_ai_reply(const String &p_text);
	void _append_chat(const String &p_text);

	// Resize drag (mouse + touch).
	void _on_resize_handle_gui_input(const Ref<InputEvent> &p_event, int p_handle);
	void _apply_resize_drag(const Vector2 &p_delta, int p_handle);

	// Structured scene actions returned by the AI as JSON.
	struct ActionResult {
		bool ok = false;
		String message;
	};
	void _execute_actions(const Array &p_actions);
	ActionResult _execute_action(const Dictionary &p_action);
	Node *_edited_scene_root() const;
	Node *_find_node_by_name(Node *p_from, const String &p_name) const;
	String _unique_child_name(Node *p_parent, const String &p_base) const;
	Vector3 _dict_vector3(const Dictionary &p_dict, const String &p_key, const Vector3 &p_default) const;
	double _dict_double(const Dictionary &p_dict, const String &p_key, double p_default) const;
	bool _dict_bool(const Dictionary &p_dict, const String &p_key, bool p_default) const;
	String _dict_string(const Dictionary &p_dict, const String &p_key, const String &p_default) const;
	void _add_node_with_undo(Node *p_parent, Node *p_node, Node *p_scene_root, const String &p_action_name);
	void _mark_scene_unsaved();
	String _motion_script_code(const String &p_mode, double p_speed, double p_amplitude, const Vector3 &p_axis) const;
	ActionResult _write_script_file(const String &p_path, const String &p_code);

protected:
	static void _bind_methods();

public:
	AIPromptDialog();
};

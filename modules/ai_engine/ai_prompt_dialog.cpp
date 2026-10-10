#include "ai_prompt_dialog.h"
#include "core/io/json.h"

void AIPromptDialog::_on_send_pressed() {
	String prompt = prompt_input->get_text();
	if (prompt.is_empty()) return;

	chat_history->set_text(chat_history->get_text() + "\nUser: " + prompt);
	prompt_input->clear();
	chat_history->set_text(chat_history->get_text() + "\nAI: Gemini প্রসেস করছে, অপেক্ষা করুন...");

	// Gemini API কল সেটআপ
	String api_key = "AQ.Ab8RN6JIUV--7s9mYIV0H7MhBNbjJ_nC1aHRdbJzXYdB4riW1Q";
	String url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=" + api_key;

	Vector<String> headers;
	headers.push_back("Content-Type: application/json");

	// প্রম্পট ডাটা তৈরি
	Dictionary content_part;
	content_part["text"] = "You are a Godot Engine AI Assistant. Generate GDScript code or game scene tree for this request: " + prompt;

	Array parts;
	parts.push_back(content_part);

	Dictionary content;
	content["parts"] = parts;

	Array contents;
	contents.push_back(content);

	Dictionary request_body;
	request_body["contents"] = contents;

	String body_json = JSON::stringify(request_body);

	// HTTP রিকোয়েস্ট পাঠানো
	if (!http_client) {
		http_client = memnew(HTTPRequest);
		add_child(http_client);
		http_client->connect("request_completed", callable_mp(this, &AIPromptDialog::_on_request_completed));
	}

	http_client->request(url, headers, HTTPClient::METHOD_POST, body_json);
}

void AIPromptDialog::_on_request_completed(int p_status, int p_code, const PackedStringArray &p_headers, const PackedByteArray &p_data) {
	String response_text;
	response_text.parse_utf8((const char *)p_data.ptr(), p_data.size());

	JSON json;
	Error err = json.parse(response_text);
	if (err == OK) {
		Dictionary res = json.get_data();
		if (res.has("candidates")) {
			Array candidates = res["candidates"];
			if (candidates.size() > 0) {
				Dictionary cand = candidates[0];
				Dictionary content = cand["content"];
				Array parts = content["parts"];
				Dictionary part = parts[0];
				String ai_reply = part["text"];
				chat_history->set_text(chat_history->get_text() + "\nGemini AI:\n" + ai_reply);
				return;
			}
		}
	}
	chat_history->set_text(chat_history->get_text() + "\nAI: উত্তর পেতে সমস্যা হয়েছে বা কোটা শেষ।");
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
	chat_history->set_text(chat_history->get_text() + "\n[অ্যাসেট সংযুক্ত হয়েছে]: " + p_path);
}

void AIPromptDialog::_bind_methods() {}

AIPromptDialog::AIPromptDialog() {
	set_title("Sohel Engine - AI Builder");
	set_min_size(Vector2i(550, 420));

	VBoxContainer *main_vbox = memnew(VBoxContainer);
	add_child(main_vbox);

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
}

#include "ai_prompt_dialog.h"

void AIPromptDialog::_on_send_pressed() {
	String prompt = prompt_input->get_text();
	if (prompt.is_empty()) return;

	chat_history->set_text(chat_history->get_text() + "\nUser: " + prompt);
	prompt_input->clear();

	chat_history->set_text(chat_history->get_text() + "\nAI: গেম অবজেক্ট এবং স্ক্রিপ্ট তৈরি করা হচ্ছে...");
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
	chat_history->set_text(chat_history->get_text() + "\n[অ্যাসেট যুক্ত হয়েছে]: " + p_path);
}

void AIPromptDialog::_bind_methods() {}

AIPromptDialog::AIPromptDialog() {
	set_title("AI Game Builder Engine");
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
	prompt_input->set_placeholder("এখানে প্রম্পট লিখুন (যেমন: Generate a 3D arena)...");
	prompt_input->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	input_bar->add_child(prompt_input);

	send_btn = memnew(Button);
	send_btn->set_text("Generate");
	send_btn->connect("pressed", callable_mp(this, &AIPromptDialog::_on_send_pressed));
	input_bar->add_child(send_btn);
}

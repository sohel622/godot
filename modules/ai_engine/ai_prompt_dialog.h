#pragma once

#include "scene/gui/dialogs.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/text_edit.h"
#include "scene/gui/button.h"
#include "scene/gui/box_container.h"
#include "scene/gui/file_dialog.h"
#include "scene/main/http_request.h"

class AIPromptDialog : public AcceptDialog {
	GDCLASS(AIPromptDialog, AcceptDialog);

private:
	TextEdit *chat_history = nullptr;
	LineEdit *prompt_input = nullptr;
	Button *attach_btn = nullptr;
	Button *send_btn = nullptr;
	FileDialog *file_dialog = nullptr;
	HTTPRequest *http_client = nullptr;

	void _on_send_pressed();
	void _on_attach_pressed();
	void _on_file_selected(const String &p_path);
	void _on_request_completed(int p_status, int p_code, const PackedStringArray &p_headers, const PackedByteArray &p_data);

protected:
	static void _bind_methods();

public:
	AIPromptDialog();
};

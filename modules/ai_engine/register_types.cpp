#include "register_types.h"

#include "ai_prompt_dialog.h"

#include "core/object/class_db.h"

void initialize_ai_engine_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	ClassDB::register_class<AIPromptDialog>();
}

void uninitialize_ai_engine_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

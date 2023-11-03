#include <map>
#include <string>

namespace Param {
	namespace ID {
		inline constexpr char* sustainState = "Sustain State";
		inline constexpr char* rise = "Rise";
		inline constexpr char* tail = "Tail";
		inline constexpr char* wet = "Wet";
		inline constexpr char* dry = "Dry";
		inline constexpr char* period = "Period";
		inline constexpr char* maxLayers = "Max Layers";
		inline constexpr char* holdToggle = "Hold to Sustain";
		inline constexpr char* forcePeriod = "Force Period";
		inline constexpr char* keycode = "Keybind";
		inline constexpr char* autoGate = "Gate";
		inline constexpr char* autoSampleLength = "Sample Length";
		inline constexpr char* autoSustain = "Smart Sustain";
		inline constexpr char* autoGateDirection = "Gate Direction";
	};

	inline const std::map<std::string, std::string> tooltip {
		{ ID::sustainState, "" },
		{ ID::rise, "" },
		{ ID::tail, "" },
		{ ID::wet, "" },
		{ ID::dry, "Hallo guten taag" },
		{ ID::period, "" },
		{ ID::maxLayers, "" },
		{ ID::holdToggle, "" },
		{ ID::forcePeriod, "" },
		{ ID::keycode, "" },
		{ ID::autoGate, "" },
		{ ID::autoSampleLength, "" },
		{ ID::autoSustain, "" },
		{ ID::autoGateDirection, "" },
	};
};
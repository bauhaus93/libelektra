// clang-format off
ksNew (16,
	keyNew (PREFIX, KEY_VALUE, "@CONFIG_FILEPATH@", KEY_END),

	keyNew (PREFIX "/array_1",
		KEY_META, "order", "0",
		KEY_META, "array", "#4",
	KEY_END),
	keyNew (PREFIX "/array_1/#0",
		KEY_VALUE, "1",
		KEY_META, "type", "long_long",
		KEY_META, "origvalue", "1",
	KEY_END),
	keyNew (PREFIX "/array_1/#1",
		KEY_VALUE, "2",
		KEY_META, "type", "long_long",
		KEY_META, "origvalue", "2",
	KEY_END),
	keyNew (PREFIX "/array_1/#2",
		KEY_VALUE, "3",
		KEY_META, "type", "long_long",
		KEY_META, "origvalue", "3",
	KEY_END),
	keyNew (PREFIX "/array_1/#3",
		KEY_VALUE, "4",
		KEY_META, "type", "long_long",
		KEY_META, "origvalue", "4",
	KEY_END),
	keyNew (PREFIX "/array_1/#4",
		KEY_VALUE, "5",
		KEY_META, "type", "long_long",
		KEY_META, "origvalue", "5",
	KEY_END),

	keyNew (PREFIX "/array_trailing_comma_1",
		KEY_META, "order", "1",
		KEY_META, "array", "#1",
	KEY_END),
	keyNew (PREFIX "/array_trailing_comma_1/#0",
		KEY_VALUE, "1",
		KEY_META, "type", "long_long",
		KEY_META, "origvalue", "1",
	KEY_END),
	keyNew (PREFIX "/array_trailing_comma_1/#1",
		KEY_VALUE, "2",
		KEY_META, "type", "long_long",
		KEY_META, "origvalue", "2",
	KEY_END),

	keyNew (PREFIX "/array_trailing_comma_2",
		KEY_META, "order", "2",
		KEY_META, "array", "#1",
	KEY_END),
	keyNew (PREFIX "/array_trailing_comma_2/#0",
		KEY_VALUE, "1",
		KEY_META, "type", "long_long",
		KEY_META, "origvalue", "1",
	KEY_END),
	keyNew (PREFIX "/array_trailing_comma_2/#1",
		KEY_VALUE, "2",
		KEY_META, "type", "long_long",
		KEY_META, "origvalue", "2",
	KEY_END),

	keyNew (PREFIX "/array_empty",
		KEY_META, "order", "3",
		KEY_META, "array", "",
	KEY_END),

KS_END)
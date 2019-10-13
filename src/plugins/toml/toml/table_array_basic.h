// clang-format off
KeySet * expected = ksNew (16,
    keyNew (PREFIX, KEY_VALUE, "@CONFIG_FILEPATH@", KEY_END),

    keyNew (PREFIX "/table_array_A", KEY_META, "array", "#2", KEY_END),

    keyNew (PREFIX "/table_array_A/#0/x", KEY_VALUE, "1", KEY_END),
    keyNew (PREFIX "/table_array_A/#0/y", KEY_VALUE, "2", KEY_END),
    keyNew (PREFIX "/table_array_A/#0/z", KEY_VALUE, "3", KEY_END),

    keyNew (PREFIX "/table_array_A/#1/x", KEY_VALUE, "1337", KEY_END),
    keyNew (PREFIX "/table_array_A/#1/y", KEY_VALUE, "666", KEY_END),
    keyNew (PREFIX "/table_array_A/#1/z", KEY_VALUE, "42", KEY_END),

    keyNew (PREFIX "/table_array_A/#2/x", KEY_VALUE, "100", KEY_END),
    keyNew (PREFIX "/table_array_A/#2/y", KEY_VALUE, "200", KEY_END),
    keyNew (PREFIX "/table_array_A/#2/z", KEY_VALUE, "300", KEY_END),

    KS_END
);

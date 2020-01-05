INCLUDES = $(GTK_CFLAGS) $(GCONF_CFLAGS) $(OSSO_CFLAGS) $(PNG_CFLAGS) \
	   $(PANGO_CFLAGS) $(MAEMO_LAUNCHER_CFLAGS)
CC=g++
lib_LIBRARIES = libbabychess.a
libbabychess_a_CFLAGS = $(INCLUDES) \
		-DLOCALEDIR=\"$(chessuilocaledir)\" \
		-DPIXMAPSDIR=\"$(pixmapsdir)\" \
                -DPREFIX=\"$(prefix)\" -I$(top_srcdir) -I..
libbabychess_a_CXXFLAGS = $(libbabychess_a_CFLAGS)

libbabychess_a_SOURCES = \
	alpha_client_image.cc alpha_client_image.hh \
	alpha_client_image_stream.cc alpha_client_image_stream.hh \
	alpha_server_image.hh alpha_server_image_stream.cc \
	alpha_server_image_stream.hh bit_board.hh \
	board.cc board.hh \
	board_image.cc board_image.hh \
	board_settings.cc board_settings.hh \
	canvas.cc canvas.hh \
	chess_color.hh client_image.cc \
	client_image.hh \
	client_image_stream.hh constant.cc \
	constant.hh \
	data.hh \
	date.cc date.hh \
	dir.cc dir.hh \
	display_type.hh \
	draw_board.cc draw_board.hh \
	dynamic.hh \
	file_name.cc file_name.hh \
	file_test.hh \
	fen.cc fen.hh \
	format.cc format.hh \
	game.cc game.hh \
	game_editor.cc game_editor.hh \
	game_info.cc game_info.hh \
	game_stream.cc game_stream.hh \
	generator.cc generator.hh \
	get_dir.cc get_dir.hh \
	gtk_object.cc gtk_object.hh \
	hash_table.cc hash_table.hh \
	has_suffix.cc has_suffix.hh \
	inline.hh \
	input_reader.cc input_reader.hh \
	int.cc int.hh \
	legal.cc legal.hh \
	material.hh \
	message_receiver.cc message_receiver.hh \
	mouse_action.hh mouse_button.hh \
	move.cc move.hh \
	move_name.hh \
	moveable.cc moveable.hh \
	moves.cc moves.hh \
	nag.cc nag.hh \
	notation.cc notation.hh \
	notation_reset.cc notation_reset.hh \
	object.cc object.hh \
	packer.cc packer.hh \
	parse_move.cc parse_move.hh \
	parser.cc parser.hh \
	paner.hh \
	piece.hh piece_type.hh \
	pgn.cc pgn.hh \
	point.hh \
	pointer.cc pointer.hh \
	position.cc position.hh \
	position_stream.cc position_stream.hh \
	printify_char.cc printify_char.hh \
	promotion_dialog.cc promotion_dialog.hh \
	read_line.cc read_line.hh \
	read_fen.cc read_fen.hh \
	rect.cc rect.hh \
	raw_pgn.cc raw_pgn.hh \
	rgb.hh \
	san.cc san.hh \
	scaled_board_image.cc scaled_board_image.hh \
	server_image.cc server_image.hh \
	server_image_stream.cc server_image_stream.hh \
	simple_notation.cc simple_notation.hh \
	square.cc square.hh \
	strdupa.cc strdupa.hh \
	stream.cc stream.hh \
	stream_file.hh stretchness.hh \
	string.cc string.hh \
	truncate.cc truncate.hh \
	text_utils.cc text_utils.hh \
	type_name.hh \
	variation.cc variation.hh \
	vector.cc vector.hh \
	widget.cc widget.hh \
	window.cc window.hh \
	notation_defaults.cc notation_defaults.hh \
	board_defaults.cc board_defaults.hh \
	label.cc label.hh \
	text_button.cc text_button.hh \
	preferences.hh rc.hh

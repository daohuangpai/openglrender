#include "fonterror.h"
#include <ft2build.h>  
#include FT_FREETYPE_H  

const char* font_error_string(int code)
{
	switch (code)
	{
	/* generic errors */
	case FT_Err_Ok:
		return "no error";
		break;
	case FT_Err_Cannot_Open_Resource:
		return "cannot open resource";
		break;
	case FT_Err_Unknown_File_Format:
		return "unknown file format";
		break;
	case FT_Err_Invalid_File_Format:
		return "broken file";
		break;
	case FT_Err_Invalid_Version:
		return "invalid FreeType version";
	case FT_Err_Lower_Module_Version:
		return "module version is too low";
		break;
	case FT_Err_Invalid_Argument:
		return "invalid argument";
		break;
	case FT_Err_Unimplemented_Feature:
		return "unimplemented feature";
		break;
	case FT_Err_Invalid_Table:
		return "broken table";
		break;
	case FT_Err_Invalid_Offset:
		return "broken offset within table";
		break;
	case FT_Err_Array_Too_Large:
		return "array allocation size too large";
		break;
	case FT_Err_Missing_Module:
		return "missing module";
		break;
	case FT_Err_Missing_Property:
		return "missing property";
		break;

	  /* glyph/character errors */
	case FT_Err_Invalid_Glyph_Index:
		return "invalid glyph index";
		break;	
	case FT_Err_Invalid_Character_Code:
		return "invalid character code";
		break;
	case FT_Err_Invalid_Glyph_Format:
		return "unsupported glyph image format";
		break;
	case FT_Err_Cannot_Render_Glyph:
		return "cannot render this glyph format";
		break;
	case FT_Err_Invalid_Outline:
		return "invalid outline";
		break;
	case FT_Err_Invalid_Composite:
		return "invalid composite glyph";
		break;
	case FT_Err_Too_Many_Hints:
		return "too many hints";
		break;
	case FT_Err_Invalid_Pixel_Size:
		return "invalid pixel size";
		break;

	/* handle errors */
	case FT_Err_Invalid_Handle:
		return "invalid object handle";
		break;
	case FT_Err_Invalid_Library_Handle:
		return "invalid library handle";
		break;
	case FT_Err_Invalid_Driver_Handle:
		return "invalid module handle";
		break;
	case FT_Err_Invalid_Face_Handle:
		return "invalid face handle";
		break;
	case FT_Err_Invalid_Size_Handle:
		return "invalid size handle";
		break;
	case FT_Err_Invalid_Slot_Handle:
		return "invalid glyph slot handle";
		break;
	case FT_Err_Invalid_CharMap_Handle:
		return "invalid charmap handle";
		break;
	case FT_Err_Invalid_Cache_Handle:
		return "invalid cache manager handle";
		break;
	case FT_Err_Invalid_Stream_Handle:
		return "invalid stream handle";
		break;

	/* driver errors */
	case FT_Err_Too_Many_Drivers:
		return "too many modules";
		break;
	case FT_Err_Too_Many_Extensions:
		return "too many extensions";
		break;
	/* memory errors */
	case FT_Err_Out_Of_Memory:
		return "out of memory";
		break;
	case FT_Err_Unlisted_Object:
		return "unlisted object";
		break;
	/* stream errors */
	case FT_Err_Cannot_Open_Stream:
		return "cannot open stream";
		break;
	case FT_Err_Invalid_Stream_Seek:
		return "invalid stream seek";
		break;
	case FT_Err_Invalid_Stream_Skip:
		return "invalid stream skip";
		break;
	case FT_Err_Invalid_Stream_Read:
		return "invalid stream read";
		break;
	case FT_Err_Invalid_Stream_Operation:
		return "invalid stream operation";
		break;
	case FT_Err_Invalid_Frame_Operation:
		return "invalid frame operation";
		break;
	case FT_Err_Nested_Frame_Access:
		return "nested frame access";
		break;
	case FT_Err_Invalid_Frame_Read:
		return "invalid frame read";
		break;
	/* raster errors */
	case FT_Err_Raster_Uninitialized:
		return "raster uninitialized";
		break;
	case FT_Err_Raster_Corrupted:
		return "raster corrupted";
		break;
	case FT_Err_Raster_Overflow:
		return "raster overflow";
		break;
	case FT_Err_Raster_Negative_Height:
		return "negative height while rastering";
		break;

	/* cache errors */
	case FT_Err_Too_Many_Caches:
		return "too many registered caches";
		break;

/* TrueType and SFNT errors */
	case FT_Err_Invalid_Opcode:
		return "invalid opcode";
		break;
	case FT_Err_Too_Few_Arguments:
		return "too few arguments";
		break;
		break;
	case FT_Err_Stack_Overflow:
		return "stack overflow";
		break;
	case FT_Err_Code_Overflow:
		return "code overflow";
		break;
	case FT_Err_Bad_Argument:
		return "bad argument";
		break;
	case FT_Err_Divide_By_Zero:
		return "division by zero";
		break;
	case FT_Err_Invalid_Reference:
		return "invalid reference";
		break;
	case FT_Err_Debug_OpCode:
		return "found debug opcode";
		break;
	case FT_Err_ENDF_In_Exec_Stream:
		return "found ENDF opcode in execution stream";
		break;
	case FT_Err_Nested_DEFS:
		return "nested DEFS";
		break;
	case FT_Err_Invalid_CodeRange:
		return "invalid code range";
		break;
	case FT_Err_Execution_Too_Long:
		return "execution context too long";
		break;
	case FT_Err_Too_Many_Function_Defs:
		return "too many function definitions";
		break;
	case FT_Err_Too_Many_Instruction_Defs:
		return "too many instruction definitions";
		break;
	case FT_Err_Table_Missing:
		return "SFNT font table missing";
		break;
	case FT_Err_Horiz_Header_Missing:
		return "horizontal header (hhea) table missing";
		break;
	case FT_Err_Locations_Missing:
		return "locations (loca) table missing";
		break;
		break;
	case FT_Err_Name_Table_Missing:
		return "name table missing";
		break;
	case FT_Err_CMap_Table_Missing:
		return "character map (cmap) table missing";
		break;
	case FT_Err_Hmtx_Table_Missing:
		return "horizontal metrics (hmtx) table missing";
		break;
	case FT_Err_Post_Table_Missing:
		return "PostScript (post) table missing";
		break;
	case FT_Err_Invalid_Horiz_Metrics:
		return "invalid horizontal metrics";
		break;
	case FT_Err_Invalid_CharMap_Format:
		return "invalid character map (cmap) format";
		break;
	case FT_Err_Invalid_PPem:
		return "invalid ppem value";
		break;
	case FT_Err_Invalid_Vert_Metrics:
		return "invalid vertical metrics";
		break;
	case FT_Err_Could_Not_Find_Context:
		return "could not find context";
		break;
	case FT_Err_Invalid_Post_Table_Format:
		return "invalid PostScript (post) table format";
		break;
	case FT_Err_Invalid_Post_Table:
		return "invalid PostScript (post) table";
		break;
	case FT_Err_DEF_In_Glyf_Bytecode:
		return "found FDEF or IDEF opcode in glyf bytecode";
		break;
	case FT_Err_Missing_Bitmap:
		return "missing bitmap in strike";
		break;

	/* CFF, CID, and Type 1 errors */
	case FT_Err_Syntax_Error:
		return "opcode syntax error";
		break;
	case FT_Err_Stack_Underflow:
		return "argument stack underflow";
		break;
	case FT_Err_Ignore:
		return "ignore";
		break;
	case FT_Err_No_Unicode_Glyph_Name:
		return "no Unicode glyph name found";
		break;
	case FT_Err_Glyph_Too_Big:
		return "glyph too big for hinting";
		break;

	/* BDF errors */
	case FT_Err_Missing_Startfont_Field:
		return "`STARTFONT' field missing";
		break;
	case FT_Err_Missing_Font_Field:
		return "`FONT' field missing";
		break;
	case FT_Err_Missing_Size_Field:
		return "`SIZE' field missing";
		break;
	case FT_Err_Missing_Fontboundingbox_Field:
		return "`FONTBOUNDINGBOX' field missing";
		break;
	case FT_Err_Missing_Chars_Field:
		return "`CHARS' field missing";
		break;
	case FT_Err_Missing_Startchar_Field:
		return "`STARTCHAR' field missing";
		break;
	case FT_Err_Missing_Encoding_Field:
		return "`ENCODING' field missing";
		break;
		break;
	case FT_Err_Missing_Bbx_Field:
		return "`BBX' field missing";
		break;
	case FT_Err_Bbx_Too_Big:
		return "`BBX' too big";
		break;
	case FT_Err_Corrupted_Font_Header:
		return "Font header corrupted or missing fields";
		break;
	case FT_Err_Corrupted_Font_Glyphs:
		return "Font glyphs corrupted or missing fields";
		break;
	default:
		break;
     }
	 return "unknow error";
}



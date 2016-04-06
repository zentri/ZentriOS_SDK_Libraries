#ifndef __JSMN_H_
#define __JSMN_H_

/**
 * JSON type identifier. Basic types are:
 * 	o Object
 * 	o Array
 * 	o String
 * 	o Other primitive: number, boolean (true/false) or null
 */
typedef enum {
	JSMN_PRIMITIVE = 0,
	JSMN_OBJECT = 1,
	JSMN_ARRAY = 2,
	JSMN_STRING = 3
} jsmntype_t;

typedef enum {
	/* Not enough tokens were provided */
	JSMN_ERROR_NOMEM = -1,
	/* Invalid character inside JSON string */
	JSMN_ERROR_INVAL = -2,
	/* The string is not a full JSON packet, more bytes expected */
	JSMN_ERROR_PART = -3,
	/* Everything was fine */
	JSMN_SUCCESS = 0
} jsmnerr_t;

/**
 * JSON token description.
 * @param		type	type (object, array, string etc.)
 * @param		start	start position in JSON data string
 * @param		end		end position in JSON data string
 */
typedef struct {
	jsmntype_t type;
	int start;
	int end;
	int size;
#ifdef JSMN_PARENT_LINKS
	int parent;
#endif
} jsmntok_t;

/**
 * JSON parser. Contains an array of token blocks available. Also stores
 * the string being parsed now and current position in that string
 */
typedef struct {
	unsigned int pos; /* offset in the JSON string */
	int toknext; /* next token to allocate */
	int toksuper; /* superior token node, e.g parent object or array */
} jsmn_parser;

/**
 * Create JSON parser over an array of tokens
 */
void jsmn_init(jsmn_parser *parser);

/**
 * Run JSON parser. It parses a JSON data string into and array of tokens, each describing
 * a single JSON object.
 */
jsmnerr_t jsmn_parse(jsmn_parser *parser, const char *js, jsmntok_t *tokens, unsigned int num_tokens);

jsmntok_t *jsmn_next(jsmntok_t *tok);
jsmntok_t *jsmn_first(jsmntok_t *tok);
int jsmn_count(jsmntok_t *tok);
int jsmn_key_count(jsmntok_t *tok);
int jsmn_array_count(jsmntok_t *tok);
const char *jsmn_value(jsmntok_t *tok, const char *value);
int jsmn_size(const jsmntok_t *tok);
// Check if JSON token extracted from 'json' is a string with value 's'.
int jsmn_is_str(jsmntok_t *tok, const char *in_json, const char *s);
const char* jsmn_copy_str(jsmntok_t *tok, char *json_data, char *buffer);
const char*jsmn_get_str(const jsmntok_t *tok, char *json_data);


#endif /* __JSMN_H_ */

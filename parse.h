


struct AstNode* parse_expression(struct Token* tokens, int token_len, int* current_token);
struct AstNode* parse_factor(struct Token* tokens, int token_len, int* current_token);
struct AstNode* parse_term(struct Token* tokens, int token_len, int* current_token);
struct AstNode* parse(struct Token* tokens, int token_len);
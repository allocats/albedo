#include "../parser.h" 

Precedence get_precedence(TokenKind kind) {
    switch (kind) {
        case T_Eq:
        case T_PlusEq:
        case T_MinusEq:
        case T_StarEq:
        case T_SlashEq:
        case T_PercentEq:
        case T_ShlEq:
        case T_ShrEq:
        case T_AndEq:
        case T_OrEq:
        case T_XorEq: {
            return PREC_ASSIGNMENT;
        }

        case T_As: {
            return PREC_CAST;
        }
        
        case T_CondOr: {
            return PREC_LOGICAL_OR;
        }
        
        case T_CondAnd: {
            return PREC_LOGICAL_AND;
        }
        
        case T_Or: {
            return PREC_BITWISE_OR;
        }
        
        case T_Xor: {
            return PREC_BITWISE_XOR;
        }
        
        case T_And: {
            return PREC_BITWISE_AND;
        }
        
        case T_BangEq:
        case T_EqEq: {
            return PREC_EQUALITY;
        }
        
        case T_Lt:
        case T_LtEq:
        case T_Gt:
        case T_GtEq: {
            return PREC_COMPARISON;
        }
        
        case T_DotDot: {
            return PREC_RANGE;
        }
        
        case T_Shr:
        case T_Shl: {
            return PREC_SHIFT;
        }
        
        case T_Plus:
        case T_Minus: {
            return PREC_ADDITIVE;
        }
        
        case T_Star:
        case T_Slash: 
        case T_Percent: {
            return PREC_MULTIPLICATIVE;
        }
        
        default: {
            return PREC_NONE;
        }
    }
}

bool is_right_associative(Precedence prec) {
    switch (prec) {
        case PREC_ASSIGNMENT:
        case PREC_CAST: {
            return true;
        } break;

        default: {
            return false;
        } break;
    }
}

bool is_unary_operator(TokenKind kind) {
    switch (kind) {
        case T_Minus:
        case T_Bang:
        case T_Not:
        case T_Star:
        case T_And: {
            return true;
        }

        default: {
            return false;
        }
    }
}

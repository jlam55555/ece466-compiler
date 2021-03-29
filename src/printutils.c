#include <stdio.h>
#include <scope.h>
#include <lexerutils/errorutils.h>
#include "astnode.h"
#include "structunion.h"
#include "printutils.h"

void print_typespec(union astnode *node)
{
	FILE *fp = stdout;
	struct astnode_typespec_scalar *sc;
	struct astnode_typespec_structunion *su;

	if (!node) {
		fprintf(fp, "<unspecified type>\n");
		return;
	}

	switch (NT(node)) {

	// scalar types
	case NT_TS_SCALAR:
		sc = &node->ts_scalar;

		// signedness (if specified)
		switch (sc->modifiers.sign) {
		case SIGN_SIGNED:	fprintf(fp, "signed "); break;
		case SIGN_UNSIGNED:	fprintf(fp, "unsigned "); break;
		}

		// long/long long/short (for applicable types)
		switch (sc->modifiers.lls) {
		case LLS_SHORT:		fprintf(fp, "short "); break;
		case LLS_LONG:		fprintf(fp, "long "); break;
		case LLS_LONG_LONG:	fprintf(fp, "long long "); break;
		}

		// "base" scalar type
		switch (sc->basetype) {
		case BT_INT: 		fprintf(fp, "int"); break;
		case BT_FLOAT: 		fprintf(fp, "float"); break;
		case BT_DOUBLE: 	fprintf(fp, "double"); break;
		case BT_CHAR: 		fprintf(fp, "char"); break;
		case BT_BOOL: 		fprintf(fp, "bool"); break;

		// should only be used in function param lists, but this
		// is not really enforced
		case BT_VOID:		fprintf(fp, "void"); break;
		}
		break;

	// struct types: only need to print tag and where it was defined
	case NT_TS_STRUCT_UNION:
		su = &node->ts_structunion;
		fprintf(fp, "struct %s ", su->ident);
		if (su->is_complete) {
			fprintf(fp, "(defined at %s:%d)",
				su->def_filename, su->def_lineno);
		} else {
			fprintf(fp, "(incomplete)");
		}
		break;

	default:
		yyerror("unknown typespec");
	}
}

void print_structunion_def(union astnode *node)
{
	FILE *fp = stdout;
	union astnode *iter;
	struct astnode_typespec_structunion *su;

	su = &node->ts_structunion;

	fprintf(fp, "struct %s definition at %s:%d{\n",
		su->ident, su->def_filename, su->def_lineno);

	// loop through fields
	iter = su->members;
	while (iter) {
		print_symbol(iter, 0);
		iter = iter->generic.next;
	}

	fprintf(fp, "}\n");
}

void print_declarator(union astnode *component, int depth)
{
	FILE *fp = stdout;
	union astnode *iter;

	// end of declarator chain
	if (!component) {
		return;
	}

	switch (NT(component)) {
	// base case
	case NT_DECL:
		print_declarator(component->decl.components, depth);
		return;

	// end of declarator chain, typespec reached
	case NT_DECLSPEC:
		print_declspec(component, depth);
		return;

	// declarator components
	case NT_DECLARATOR_ARRAY:
		INDENT(depth);
		fprintf(fp, "array (%d) of\n",
			component->decl_array.length->num.num.int_val);
		break;
	case NT_DECLARATOR_POINTER:
		INDENT(depth);
		print_typequal(component->decl_pointer.spec);
		fprintf(fp, "pointer to\n");
		break;
	case NT_DECLARATOR_FUNCTION:
		INDENT(depth);
		fprintf(fp, "function\n");
		++depth;
		INDENT(depth);
		fprintf(fp, "with arguments\n");

		// print out argument list
		if (!component->decl_function.paramlist) {
			INDENT(depth+1);
			fprintf(fp, "<unknown>\n");
		} else {
			LL_FOR(component->decl_function.paramlist, iter) {
				// TODO: handle ... and void in paramlist
				print_declarator(iter, depth+1);
			}
		}

		INDENT(depth);
		fprintf(fp, "returning\n");
		break;
	default:
		fprintf(fp, "unknown type %d in print_symbol\n", NT(component));
		return;
	}

	// recursively print
	print_declarator(LL_NEXT_OF(component), depth+1);
}

void print_typequal(union astnode *node)
{
	FILE *fp = stdout;
	unsigned char tq;

	if (!node) {
		return;
	}

	tq = node->tq.qual;
	if (tq & TQ_CONST)	fprintf(fp, "const ");
	if (tq & TQ_RESTRICT)	fprintf(fp, "restrict ");
	if (tq & TQ_VOLATILE)	fprintf(fp, "volatile ");
}

void print_storageclass(union astnode *node)
{
	FILE *fp = stdout;
	enum sc_spec scspec;

	if (!node) {
		// unspecified storage class
		return;
	}

	scspec = node->sc.scspec;

	switch (scspec) {
	case SC_EXTERN:		fprintf(fp, "extern "); return;
	case SC_STATIC:		fprintf(fp, "static "); return;
	case SC_AUTO:		fprintf(fp, "auto "); return;
	case SC_REGISTER:	fprintf(fp, "register "); return;
	}
}

void print_declspec(union astnode *node, int depth)
{
	FILE *fp = stdout;

	if (!node) {
		// shouldn't happen
		yyerror("missing declspec?");
		return;
	}

	INDENT(depth);
	print_typequal(node->declspec.tq);
	print_typespec(node->declspec.ts);
	fprintf(fp, "\n");
}

void print_scope(struct scope *scope)
{
	FILE *fp = stdout;
	char *type;

	switch (scope->type) {
	case ST_FILE:		type = "global"; break;
	case ST_FUNC:		type = "function"; break;
	case ST_BLOCK:		type = "block"; break;
	// "fake" scope for printing purposes
	case ST_STRUCTUNION:	type = "struct/union"; break;
	default:		yyerror_fatal("unknown scope type");
	}

	fprintf(fp, "%s scope starting at %s:%d",
		type, scope->filename, scope->lineno);
}

void print_symbol(union astnode *node, int print_sc)
{
	FILE *fp = stdout;

	if (!node || NT(node) != NT_DECL) {
		return;
	}

	fprintf(fp, "%s is defined ", node->decl.ident);
	if (print_sc) {
		fprintf(fp, "with storage class ");
		print_storageclass(node->decl.declspec->declspec.sc);
	}
	fprintf(fp, "\n");
	INDENT(1);
	fprintf(fp, "at %s:%d [in ", filename, lineno);
	print_scope(get_current_scope());
	fprintf(fp, "] as a\n");

	// print declarator and type
	print_declarator(node->decl.components, 2);
}
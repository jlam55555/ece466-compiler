/**
 * 	Collection of functions to print out various internal representations.
 */

#ifndef PRINTUTILSH
#define PRINTUTILSH

#include <parser/astnode.h>
#include <parser/scope.h>

/**
 * print out struct/union definition recursively
 *
 * @param node 	struct/union typespec
 */
void print_structunion_def(union astnode *node);

/**
 * print out typespec (scalar or struct/union)
 *
 * @param node		astnode_ts_* object
 */
void print_typespec(union astnode *node);

/**
 * recursively print declarator components; initiate by calling on the
 * component list of a astnode_decl object
 *
 * @param component	ll of astnode_decl_*
 * @param depth		indenting depth
 */
void print_declarator(union astnode *component, int depth);

/**
 * print type qualifier list
 *
 * @param node 		astnode_typequal object
 */
void print_typequal(union astnode *node);

/**
 * print storage class
 *
 * @param node		astnode_storageclass object
 */
void print_storageclass(union astnode *node);

/**
 * print typespec and typequal (but not storagespec) at given indenting depth
 *
 * @param node		astnode_declspec object
 * @param depth		indenting depth
 */
void print_declspec(union astnode *node, int depth);

/**
 * print scope
 *
 * @param scope 	scope object
 */
void print_scope(struct scope *scope);

/**
 * print variable: declaration specifiers, scope, and declarator
 *
 * @param node 		astnode_decl (declaration) object
 * @param is_not_member	boolean indicating whether is not struct union member
 * 			(e.g., for struct/union don't print storage class,
 * 			 don't lookup scope)
 * @param depth		indenting depth
 */
void print_symbol(union astnode *node, int is_not_member, int depth);

/**
 * print expression
 * 
 * @param node		astnode representation of expression
 * @param depth		indenting depth
 */
void print_expr(union astnode *node, int depth);

#endif	// PRINTUTILSH
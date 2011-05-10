/*
 * FDDL : The Free Decision Diagram Library 
 * Copyright (C) 2004 Robert Marmorstein
 * 
 * This program is released under the GNU Public License, version 2.  Please
 * see the file "LICENSE" in the root directory of this repository for more
 * information.
 */

#include "uniquetable.h"
#include "dynarray.h"


UniqueTable::UniqueTable(int K, hfunc h, cfunc c) : m_numlevels(K + 1), m_hashfunc(h), m_compare(c)
{
    m_table = new table_node **[m_numlevels];
    for (unsigned int i = 0; i < m_numlevels; i++) {
	m_table[i] = new table_node *[TABLE_SIZE];
	for (unsigned int j = 0; j < TABLE_SIZE; j++) {
	    m_table[i][j] = NULL;
	}
    }
}

UniqueTable::~UniqueTable()
{
    table_node *cur;

    for (unsigned int i = 0; i < m_numlevels; i++) {
	for (unsigned int j = 0; j < TABLE_SIZE; j++) {
	    while (m_table[i][j] != NULL) {
		cur = m_table[i][j];
		m_table[i][j] = cur->next;
		delete cur;
	    }
	}
	delete[]m_table[i];
    }
    delete[]m_table;
}

node_idx UniqueTable::look_up(level k, node_idx p)
{
    table_node *cur;
    unsigned int idx;

    idx = m_hashfunc(k, p) % TABLE_SIZE;
    cur = m_table[k][idx];
    while (cur != NULL) {
	if (m_compare(k, p, cur->p) == 1) {
	    return cur->p;
	}
	cur = cur->next;
    }
    return -1;
}

node_idx UniqueTable::add(level k, node_idx p)
{
    node_idx r;
    table_node *newNode;
    unsigned int idx;

    r = look_up(k, p);
    if (r != -1)
	return r;

    idx = m_hashfunc(k, p) % TABLE_SIZE;

    newNode = new table_node;
    newNode->k = k;
    newNode->p = p;
    newNode->next = m_table[k][idx];
    m_table[k][idx] = newNode;
    return p;
}

int UniqueTable::remove(level k, node_idx p)
{
    table_node *cur;
    table_node *prev;
    unsigned int idx;

    idx = m_hashfunc(k, p) % TABLE_SIZE;

    prev = NULL;
    cur = m_table[k][idx];

    while (cur != NULL) {
	if (m_compare(k, p, cur->p) == 1) {
	    if (prev == NULL) {
		m_table[k][idx] = cur->next;
		delete cur;
	    } else {
		prev->next = cur->next;
		delete cur;
	    }
	    return 1;
	}
	prev = cur;
	cur = cur->next;
    }
    return 0;
}

int UniqueTable::remap(level k, DynArray < node_idx > *transTable)
{
    int i;
    table_node *cur;
    node_idx newP;

    for (i = 0; i < TABLE_SIZE; i++) {
	cur = m_table[k][i];
	while (cur != NULL) {
	    newP = (*(*transTable)[cur->p]);
	    cur->p = newP;
	    cur = cur->next;
	}
    }
}

/*
** $Id: ltable.h $
** Lua tables (hash)
** See Copyright Notice in lua.h
*/

#ifndef ltable_h
#define ltable_h

#include "lobject.h"


#define gnode(t,i)	(&(t)->node[i])
#define gval(n)		(&(n)->i_val)
#define gnext(n)	((n)->u.next)


/*
** Clear all bits of fast-access metamethods, which means that the table
** may have any of these metamethods. (First access that fails after the
** clearing will set the bit again.)
*/
#define invalidateTMcache(t)	((t)->flags &= ~maskflags)


/*
** Bit BITDUMMY set in 'flags' means the table is using the dummy node
** for its hash part.
*/

#define BITDUMMY		(1 << 6)
#define NOTBITDUMMY		cast_byte(~BITDUMMY)
#define isdummy(t)		((t)->flags & BITDUMMY)

#define setnodummy(t)		((t)->flags &= NOTBITDUMMY)
#define setdummy(t)		((t)->flags |= BITDUMMY)



/* allocated size for hash nodes */
#define allocsizenode(t)	(isdummy(t) ? 0 : sizenode(t))


/* returns the Node, given the value of a table entry */
#define nodefromval(v)	cast(Node *, (v))



#define luaH_fastgeti(t,k,res,hres) \
  { Table *h = t; lua_Unsigned u = l_castS2U(k); \
    if ((u - 1u < h->alimit)) { \
      int tag = *getArrTag(h,(u)-1u); \
      if (tagisempty(tag)) hres = HNOTFOUND; \
      else { farr2val(h, u, tag, res); hres = HOK; }} \
    else { hres = luaH_getint(h, u, res); }}


#define luaH_fastseti(t,k,val,hres) \
  { Table *h = t; lua_Unsigned u = l_castS2U(k); \
    if ((u - 1u < h->alimit)) { \
      lu_byte *tag = getArrTag(h,(u)-1u); \
      if (tagisempty(*tag)) hres = ~cast_int(u); \
      else { fval2arr(h, u, tag, val); hres = HOK; }} \
    else { hres = luaH_psetint(h, u, val); }}


/* results from get/pset */
#define HOK		0
#define HNOTFOUND	1
#define HNOTATABLE	2
#define HFIRSTNODE	3

/*
** 'luaH_get*' operations set 'res' and return HOK, unless the value is
** absent. In that case, they set nothing and return HNOTFOUND.
** The 'luaH_pset*' (pre-set) operations set the given value and return
** HOK, unless the original value is absent. In that case, if the key
** is really absent, they return HNOTFOUND. Otherwise, if there is a
** slot with that key but with no value, 'luaH_pset*' return an encoding
** of where the key is (usually called 'hres'). (pset cannot set that
** value because there might be a metamethod.) If the slot is in the
** hash part, the encoding is (HFIRSTNODE + hash index); if the slot is
** in the array part, the encoding is (~array index), a negative value.
** The value HNOTATABLE is used by the fast macros to signal that the
** value being indexed is not a table.
*/


/*
** The array part of a table is represented by an array of *cells*.
** Each cell is composed of NM tags followed by NM values, so that
** no space is wasted in padding.
*/
#define NM      cast_uint(sizeof(Value))


/*
** A few operations on arrays can be performed "in bulk", treating all
** tags of a cell as a simple (or a few) word(s). The next constant is
** the number of words to cover the tags of a cell. (In conventional
** architectures that will be 1 or 2.)
*/
#define BKSZ	cast_int((NM - 1) / sizeof(lua_Unsigned) + 1)

struct ArrayCell {
  union {
    lua_Unsigned bulk[BKSZ];  /* for "bulk" operations */
    lu_byte tag[NM];
  } u;
  Value value[NM];
};


/* Computes the address of the tag for the abstract index 'k' */
#define getArrTag(t,k)	(&(t)->array[(k)/NM].u.tag[(k)%NM])

/* Computes the address of the value for the abstract index 'k' */
#define getArrVal(t,k)	(&(t)->array[(k)/NM].value[(k)%NM])


/*
** Move TValues to/from arrays, using Lua indices
*/
#define arr2obj(h,k,val)  \
  ((val)->tt_ = *getArrTag(h,(k)-1u), (val)->value_ = *getArrVal(h,(k)-1u))

#define obj2arr(h,k,val)  \
  (*getArrTag(h,(k)-1u) = (val)->tt_, *getArrVal(h,(k)-1u) = (val)->value_)


/*
** Often, we need to check the tag of a value before moving it. These
** macros also move TValues to/from arrays, but receive the precomputed
** tag value or address as an extra argument.
*/
#define farr2val(h,k,tag,res)  \
  ((res)->tt_ = tag, (res)->value_ = *getArrVal(h,(k)-1u))

#define fval2arr(h,k,tag,val)  \
  (*tag = (val)->tt_, *getArrVal(h,(k)-1u) = (val)->value_)


LUAI_FUNC int luaH_get (Table *t, const TValue *key, TValue *res);
LUAI_FUNC int luaH_getshortstr (Table *t, TString *key, TValue *res);
LUAI_FUNC int luaH_getstr (Table *t, TString *key, TValue *res);
LUAI_FUNC int luaH_getint (Table *t, lua_Integer key, TValue *res);

/* Special get for metamethods */
LUAI_FUNC const TValue *luaH_Hgetshortstr (Table *t, TString *key);

LUAI_FUNC TString *luaH_getstrkey (Table *t, TString *key);

LUAI_FUNC int luaH_psetint (Table *t, lua_Integer key, TValue *val);
LUAI_FUNC int luaH_psetshortstr (Table *t, TString *key, TValue *val);
LUAI_FUNC int luaH_psetstr (Table *t, TString *key, TValue *val);
LUAI_FUNC int luaH_pset (Table *t, const TValue *key, TValue *val);

LUAI_FUNC void luaH_setint (lua_State *L, Table *t, lua_Integer key,
                                                    TValue *value);
LUAI_FUNC void luaH_set (lua_State *L, Table *t, const TValue *key,
                                                 TValue *value);

LUAI_FUNC void luaH_finishset (lua_State *L, Table *t, const TValue *key,
                                              TValue *value, int hres);
LUAI_FUNC Table *luaH_new (lua_State *L);
LUAI_FUNC void luaH_resize (lua_State *L, Table *t, unsigned int nasize,
                                                    unsigned int nhsize);
LUAI_FUNC void luaH_resizearray (lua_State *L, Table *t, unsigned int nasize);
LUAI_FUNC void luaH_free (lua_State *L, Table *t);
LUAI_FUNC int luaH_next (lua_State *L, Table *t, StkId key);
LUAI_FUNC lua_Unsigned luaH_getn (Table *t);
LUAI_FUNC unsigned int luaH_realasize (const Table *t);


#if defined(LUA_DEBUG)
LUAI_FUNC Node *luaH_mainposition (const Table *t, const TValue *key);
#endif


#endif

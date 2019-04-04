/* token */

enum tokenkind {
	TNONE,

	TEOF,
	TNEWLINE,

	TIDENT,
	TNUMBER,
	TCHARCONST,
	TSTRINGLIT,

	/* keyword */
	TAUTO,
	TBREAK,
	TCASE,
	TCHAR,
	TCONST,
	TCONTINUE,
	TDEFAULT,
	TDO,
	TDOUBLE,
	TELSE,
	TENUM,
	TEXTERN,
	TFLOAT,
	TFOR,
	TGOTO,
	TIF,
	TINLINE,
	TINT,
	TLONG,
	TREGISTER,
	TRESTRICT,
	TRETURN,
	TSHORT,
	TSIGNED,
	TSIZEOF,
	TSTATIC,
	TSTRUCT,
	TSWITCH,
	TTYPEDEF,
	TUNION,
	TUNSIGNED,
	TVOID,
	TVOLATILE,
	TWHILE,
	T_ALIGNAS,
	T_ALIGNOF,
	T_ATOMIC,
	T_BOOL,
	T_COMPLEX,
	T_GENERIC,
	T_IMAGINARY,
	T_NORETURN,
	T_STATIC_ASSERT,
	T_THREAD_LOCAL,
	T__TYPEOF__,

	/* punctuator */
	TLBRACK,
	TRBRACK,
	TLPAREN,
	TRPAREN,
	TLBRACE,
	TRBRACE,
	TPERIOD,
	TARROW,
	TINC,
	TDEC,
	TBAND,
	TMUL,
	TADD,
	TSUB,
	TBNOT,
	TLNOT,
	TDIV,
	TMOD,
	TSHL,
	TSHR,
	TLESS,
	TGREATER,
	TLEQ,
	TGEQ,
	TEQL,
	TNEQ,
	TXOR,
	TBOR,
	TLAND,
	TLOR,
	TQUESTION,
	TCOLON,
	TSEMICOLON,
	TELLIPSIS,
	TASSIGN,
	TMULASSIGN,
	TDIVASSIGN,
	TMODASSIGN,
	TADDASSIGN,
	TSUBASSIGN,
	TSHLASSIGN,
	TSHRASSIGN,
	TBANDASSIGN,
	TXORASSIGN,
	TBORASSIGN,
	TCOMMA,
	THASH,
	THASHHASH,
};

struct location {
	const char *file;
	size_t line, col;
};

struct token {
	enum tokenkind kind;
	struct location loc;
	char *lit;
};

extern struct token tok;

void tokprint(const struct token *);
_Noreturn void error(const struct location *, const char *, ...);
struct token;

/* scan */

int scanfrom(const char *file);
void scan(struct token *);
struct location;

void ppinit(const char *);

void next(void);
_Bool peek(int);
char *expect(int, const char *);
_Bool consume(int);

/* type */

enum typequal {
	QUALNONE,

	QUALCONST    = 1<<1,
	QUALRESTRICT = 1<<2,
	QUALVOLATILE = 1<<3,
	QUALATOMIC   = 1<<4,
};

enum typekind {
	TYPENONE,

	TYPEQUALIFIED,
	TYPEVOID,
	TYPEBASIC,
	TYPEPOINTER,
	TYPEARRAY,
	TYPEFUNC,
	TYPESTRUCT,
	TYPEUNION,
};

enum typeprop {
	PROPNONE,

	PROPOBJECT  = 1<<0,
	PROPCHAR    = 1<<1,
	PROPINT     = 1<<2,
	PROPREAL    = 1<<3,
	PROPARITH   = 1<<4,
	PROPSCALAR  = 1<<5,
	PROPAGGR    = 1<<6,
	PROPDERIVED = 1<<7,
	PROPFLOAT   = 1<<8,
};

struct param {
	char *name;
	struct type *type;
	struct value *value;
	struct param *next;
};

struct bitfield {
	short before;  /* number of bits in the storage unit before the bit-field */
	short after;   /* number of bits in the storage unit after the bit-field */
};

struct member {
	char *name;
	struct type *type;
	uint64_t offset;
	struct bitfield bits;
	struct member *next;
};

struct type {
	enum typekind kind;
	int align;
	uint64_t size;
	struct repr *repr;
	union {
		struct type *base;
		struct list link;  /* used only during construction of type */
	};
	_Bool incomplete;
	union {
		struct {
			enum typequal kind;
		} qualified;
		struct {
			enum {
				BASICBOOL,
				BASICCHAR,
				BASICSHORT,
				BASICINT,
				BASICENUM,
				BASICLONG,
				BASICLONGLONG,
				BASICFLOAT,
				BASICDOUBLE,
				BASICLONGDOUBLE,
			} kind;
			_Bool issigned, iscomplex;
		} basic;
		struct {
			uint64_t length;
		} array;
		struct {
			_Bool isprototype, isvararg, isnoreturn, paraminfo;
			struct param *params;
		} func;
		struct {
			char *tag;
			struct member *members;
		} structunion;
	};
};

struct type *mktype(enum typekind);
struct type *mkqualifiedtype(struct type *, enum typequal);
struct type *mkpointertype(struct type *);
struct type *mkarraytype(struct type *, uint64_t);

_Bool typecompatible(struct type *, struct type *);
_Bool typesame(struct type *, struct type *);
struct type *typecomposite(struct type *, struct type *);
struct type *typeunqual(struct type *, enum typequal *);
struct type *typecommonreal(struct type *, struct type *);
struct type *typeargpromote(struct type *);
struct type *typeintpromote(struct type *);
enum typeprop typeprop(struct type *);
struct member *typemember(struct type *, const char *, uint64_t *);

struct param *mkparam(char *, struct type *);

extern struct type typevoid;
extern struct type typebool;
extern struct type typechar, typeschar, typeuchar;
extern struct type typeshort, typeushort;
extern struct type typeint, typeuint;
extern struct type typelong, typeulong;
extern struct type typellong, typeullong;
extern struct type typefloat, typedouble, typelongdouble;
extern struct type typevalist, typevalistptr;

/* decl */

enum declkind {
	DECLTYPE,
	DECLOBJECT,
	DECLFUNC,
	DECLCONST,
	DECLBUILTIN,
};

enum linkage {
	LINKNONE,
	LINKINTERN,
	LINKEXTERN,
};

enum builtinkind {
	BUILTINALLOCA,
	BUILTINCONSTANTP,
	BUILTININFF,
	BUILTINNANF,
	BUILTINOFFSETOF,
	BUILTINVAARG,
	BUILTINVACOPY,
	BUILTINVAEND,
	BUILTINVALIST,
	BUILTINVASTART,
};

struct decl {
	enum declkind kind;
	enum linkage linkage;
	struct type *type;
	struct value *value;

	/* objects and functions */
	struct list link;
	int align;  /* may be more strict than type requires */
	_Bool tentative, defined;

	/* built-ins */
	enum builtinkind builtin;
};

struct scope;
struct func;

struct decl *mkdecl(enum declkind, struct type *, enum linkage);
_Bool decl(struct scope *, struct func *);
struct type *typename(struct scope *);

struct expr;
struct decl *stringdecl(struct expr *);

void emittentativedefns(void);

/* scope */

struct scope {
	struct hashtable *tags;
	struct hashtable *decls;
	struct value *breaklabel;
	struct value *continuelabel;
	struct switchcases *switchcases;
	struct scope *parent;
};

void scopeinit(void);
struct scope *mkscope(struct scope *);
struct scope *delscope(struct scope *);

struct decl;
void scopeputdecl(struct scope *, const char *, struct decl *);
struct decl *scopegetdecl(struct scope *, const char *, _Bool);

struct type;
void scopeputtag(struct scope *, const char *, struct type *);
struct type *scopegettag(struct scope *, const char *, _Bool);

extern struct scope filescope;

/* expr */

enum exprkind {
	/* primary expression */
	EXPRIDENT,
	EXPRCONST,
	EXPRSTRING,

	/* postfix expression */
	EXPRCALL,
	/* member E.M gets transformed to *(typeof(E.M) *)((char *)E + offsetof(typeof(E), M)) */
	EXPRINCDEC,
	EXPRCOMPOUND,
	/* subscript E1[E2] gets transformed to *((E1)+(E2)) */

	EXPRUNARY,
	EXPRCAST,
	EXPRBINARY,
	EXPRCOND,
	EXPRASSIGN,
	EXPRCOMMA,

	EXPRBUILTIN,
	EXPRTEMP,
};

enum exprflags {
	EXPRFLAG_LVAL    = 1<<0,
	EXPRFLAG_DECAYED = 1<<1,
};

struct expr {
	enum exprkind kind;
	enum exprflags flags;
	struct type *type;
	struct expr *next;
	union {
		struct {
			struct decl *decl;
		} ident;
		union {
			uint64_t i;
			double f;
		} constant;
		struct {
			char *data;
			size_t size;
		} string;
		struct {
			struct expr *func, *args;
			size_t nargs;
		} call;
		struct {
			struct init *init;
		} compound;
		struct {
			int op;
			_Bool post;
			struct expr *base;
		} incdec;
		struct {
			int op;
			struct expr *base;
		} unary;
		struct {
			struct expr *e;
		} cast;
		struct {
			int op;
			struct expr *l, *r;
		} binary;
		struct {
			struct expr *e, *t, *f;
		} cond;
		struct {
			struct expr *l, *r;
		} assign;
		struct {
			struct expr *exprs;
		} comma;
		struct {
			int kind;
			struct expr *arg;
		} builtin;
		struct value *temp;
	};
};

struct scope;

struct expr *expr(struct scope *);
struct expr *assignexpr(struct scope *);
uint64_t intconstexpr(struct scope *, _Bool);
void delexpr(struct expr *);

struct expr *exprconvert(struct expr *, struct type *);

/* eval */

struct expr *eval(struct expr *);

/* init */

struct init {
	uint64_t start, end;
	struct expr *expr;
	struct init *next;
};

struct scope;
struct type;

struct init *mkinit(uint64_t, uint64_t, struct expr *);
struct init *parseinit(struct scope *, struct type *);
struct func;
struct scope;

void stmt(struct func *, struct scope *);

/* backend */

struct gotolabel {
	struct value *label;
	_Bool defined;
};

struct switchcases {
	void *root;
	struct value *defaultlabel;
};

struct repr;
struct decl;
struct expr;
struct init;
struct scope;
struct type;

struct switchcases *mkswitch(void);
void switchcase(struct switchcases *, uint64_t, struct value *);

struct value *mkblock(char *);
struct value *mkglobal(char *, _Bool);
struct value *mkintconst(struct repr *, uint64_t);

uint64_t intconstvalue(struct value *);

struct func *mkfunc(char *, struct type *, struct scope *);
struct type *functype(struct func *);
void funclabel(struct func *, struct value *);
struct value *funcexpr(struct func *, struct expr *);
void funcjmp(struct func *, struct value *);
void funcjnz(struct func *, struct value *, struct value *, struct value *);
void funcret(struct func *, struct value *);
struct gotolabel *funcgoto(struct func *, char *);
void funcswitch(struct func *, struct value *, struct switchcases *, struct value *);
void funcinit(struct func *, struct decl *, struct init *);

void emitfunc(struct func *, _Bool);
void emitdata(struct decl *,  struct init *);

extern struct repr i8, i16, i32, i64, f32, f64;
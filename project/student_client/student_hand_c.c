

/* this ALWAYS GENERATED file contains the RPC client stubs */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Thu Mar 02 10:19:24 2017
 */
/* Compiler settings for student_hand.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0603 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#if !defined(_M_IA64) && !defined(_M_AMD64) && !defined(_ARM_)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */

#pragma optimize("", off ) 

#include <string.h>

#include "student_hand_h.h"

#define TYPE_FORMAT_STRING_SIZE   7                                 
#define PROC_FORMAT_STRING_SIZE   61                                
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _student_hand_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } student_hand_MIDL_TYPE_FORMAT_STRING;

typedef struct _student_hand_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } student_hand_MIDL_PROC_FORMAT_STRING;

typedef struct _student_hand_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } student_hand_MIDL_EXPR_FORMAT_STRING;


static const RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const student_hand_MIDL_TYPE_FORMAT_STRING student_hand__MIDL_TypeFormatString;
extern const student_hand_MIDL_PROC_FORMAT_STRING student_hand__MIDL_ProcFormatString;
extern const student_hand_MIDL_EXPR_FORMAT_STRING student_hand__MIDL_ExprFormatString;

#define GENERIC_BINDING_TABLE_SIZE   0            


/* Standard interface: StudentHand, ver. 1.0,
   GUID={0x4556509F,0x618A,0x46CF,{0xAB,0x3D,0xED,0x73,0x6E,0xD6,0x64,0x76}} */

handle_t Handup_Binding;


static const RPC_CLIENT_INTERFACE StudentHand___RpcClientInterface =
    {
    sizeof(RPC_CLIENT_INTERFACE),
    {{0x4556509F,0x618A,0x46CF,{0xAB,0x3D,0xED,0x73,0x6E,0xD6,0x64,0x76}},{1,0}},
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    0,
    0,
    0,
    0,
    0x00000000
    };
RPC_IF_HANDLE StudentHand_v1_0_c_ifspec = (RPC_IF_HANDLE)& StudentHand___RpcClientInterface;

extern const MIDL_STUB_DESC StudentHand_StubDesc;

static RPC_BINDING_HANDLE StudentHand__MIDL_AutoBindHandle;


/* [async] */ void  Handup( 
    /* [in] */ PRPC_ASYNC_STATE Handup_AsyncHandle,
    /* [string][in] */ unsigned char *start)
{

    NdrAsyncClientCall(
                      ( PMIDL_STUB_DESC  )&StudentHand_StubDesc,
                      (PFORMAT_STRING) &student_hand__MIDL_ProcFormatString.Format[0],
                      ( unsigned char * )&Handup_AsyncHandle);
    
}


/* [async] */ void  HandupEnd( 
    /* [in] */ PRPC_ASYNC_STATE HandupEnd_AsyncHandle,
    /* [string][in] */ unsigned char *end)
{

    NdrAsyncClientCall(
                      ( PMIDL_STUB_DESC  )&StudentHand_StubDesc,
                      (PFORMAT_STRING) &student_hand__MIDL_ProcFormatString.Format[30],
                      ( unsigned char * )&HandupEnd_AsyncHandle);
    
}


#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT50_OR_LATER)
#error You need Windows 2000 or later to run this stub because it uses these features:
#error   [async] attribute, /robust command line switch.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will fail with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const student_hand_MIDL_PROC_FORMAT_STRING student_hand__MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure Handup */

			0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x0 ),	/* 0 */
/*  8 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 10 */	NdrFcShort( 0x0 ),	/* 0 */
/* 12 */	NdrFcShort( 0x0 ),	/* 0 */
/* 14 */	0xc2,		/* Oi2 Flags:  clt must size, has ext, has async handle */
			0x1,		/* 1 */
/* 16 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x0 ),	/* 0 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter start */

/* 24 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 26 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 28 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Procedure HandupEnd */

/* 30 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 32 */	NdrFcLong( 0x0 ),	/* 0 */
/* 36 */	NdrFcShort( 0x1 ),	/* 1 */
/* 38 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 40 */	NdrFcShort( 0x0 ),	/* 0 */
/* 42 */	NdrFcShort( 0x0 ),	/* 0 */
/* 44 */	0xc2,		/* Oi2 Flags:  clt must size, has ext, has async handle */
			0x1,		/* 1 */
/* 46 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 48 */	NdrFcShort( 0x0 ),	/* 0 */
/* 50 */	NdrFcShort( 0x0 ),	/* 0 */
/* 52 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter end */

/* 54 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 56 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 58 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

			0x0
        }
    };

static const student_hand_MIDL_TYPE_FORMAT_STRING student_hand__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/*  4 */	
			0x22,		/* FC_C_CSTRING */
			0x5c,		/* FC_PAD */

			0x0
        }
    };

static const unsigned short StudentHand_FormatStringOffsetTable[] =
    {
    0,
    30
    };


static const MIDL_STUB_DESC StudentHand_StubDesc = 
    {
    (void *)& StudentHand___RpcClientInterface,
    MIDL_user_allocate,
    MIDL_user_free,
    &Handup_Binding,
    0,
    0,
    0,
    0,
    student_hand__MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x50002, /* Ndr library version */
    0,
    0x800025b, /* MIDL Version 8.0.603 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0
    };
#pragma optimize("", on )
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64) && !defined(_ARM_) */


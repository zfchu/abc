/**CFile****************************************************************

  FileName    [imgCmd.c]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName []

  Synopsis    []

  Author      [Zhufei Chu]
  
  Affiliation [Ningbo University, China]

***********************************************************************/

#include "base/main/main.h"
#include "img.h"

ABC_NAMESPACE_IMPL_START


////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

static int img_command( Abc_Frame_t * pAbc, int argc, int ** argv );

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/

void Img_Init( Abc_Frame_t * pAbc ){
  Cmd_CommandAdd( pAbc, "NBU", "img", img_command, 0 );
}

int img_command( Abc_Frame_t * pAbc, int argc, int ** argv ){
  int fVerbose;
  int c, result;

  fVerbose = 0;

  //get arguments
  Extra_UtilGetoptReset();

  while( ( c = Extra_UtilGetopt( argc, argv, "vh" ) ) != EOF ) {
    switch( c ) {
      case 'v':
        fVerbose ^= 1;
        break;

      case 'h':
        goto usage;

      default:
        goto usage;
    }
  }

  //call the main function
    result = Img_RunMain( pAbc );

    return 0;

    usage:
    Abc_Print( -2, "usage: img [-vh] \n" );
    Abc_Print( -2, "\t print the information of the img network\n " );
    Abc_Print( -2, "\t -v : toggle printing verbose information\n " );
    Abc_Print( -2, "\t -h : print the command usage\n " );
    return 1;
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


ABC_NAMESPACE_IMPL_END


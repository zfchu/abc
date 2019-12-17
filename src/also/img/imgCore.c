/**CFile****************************************************************

  FileName    [imgCore.c]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [IMG]

  Synopsis    [Implication logic network core]

  Author      [Zhufei Chu]
  
  Affiliation [Ningbo University, China]

***********************************************************************/

#include "base/main/main.h"
#include "base/abc/abc.h"
#include "map/mio/mio.h"

ABC_NAMESPACE_IMPL_START

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

int printImgInfo( Abc_Ntk_t * pNtk );
Abc_Ntk_t * AigMap2Img( Abc_Ntk_t * pNtk );
Abc_Ntk_t * Truth2Aig( char * pStr );
Abc_Ntk_t * AigScriptOpt( Abc_Ntk_t * pNtk );

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Img_RunMain( Abc_Frame_t * pAbc ) 
{
  Abc_Ntk_t * pNtk;

  pNtk = Abc_FrameReadNtk( pAbc );

  if( pNtk == NULL ){
    Abc_Print( -1, " Getting the target network has failed.\n " );
    return 0;
  }

  //result = printImgInfo( pNtk );
  printImgInfo( Truth2Aig("12345678") );
  printImgInfo( AigScriptOpt( Truth2Aig("12345678") ) );
  //Abc_Ntk_t * pNtkRes = AigMap2Img( pNtk );
  Abc_Ntk_t * pNtkRes = AigMap2Img( AigScriptOpt( Truth2Aig( "12345678" ) ) );

  Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );

  return 1;
}
 
/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/

int printImgInfo( Abc_Ntk_t * pNtk ) 
{

  //check if the network is strashed
  if( !Abc_NtkIsStrash( pNtk ) ){
    Abc_Print( -1, " img: This command is only applicable to strashed networks.\n " );
    return 0;
  }

  //print information of the implication logic network
  Abc_Print( 1, "\t- Type:  %d\n", pNtk->ntkType );
  Abc_Print( 1, "\t- Level: %d\n", Abc_AigLevel( pNtk ) );
  Abc_Print( 1, "\t- %d primary inputs;\n", Abc_NtkPiNum( pNtk ) );
  Abc_Print( 1, "\t- %d primary outputs;\n", Abc_NtkPoNum( pNtk ) );
  Abc_Print( 1, "\t- %d AND gates.\n", Abc_NtkNodeNum( pNtk ) );

  return 1;
}

/**Function*************************************************************

  Synopsis    [aig optimization]

  Description [resyn: b; rw; rwz; b; rwz; b ]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Abc_Ntk_t * AigScriptOpt( Abc_Ntk_t * pNtk )
{
  assert( pNtk != NULL );
  assert( Abc_NtkIsStrash( pNtk ) );

  //balance parameters
  int fDuplicate   = 0;
  int fSelective   = 0;
  int fUpdateLevel = 1;

  Abc_Ntk_t * pNtkRes, * pNtkTemp;

  //balance
  pNtkTemp = Abc_NtkBalance( pNtk, fDuplicate, fSelective, fUpdateLevel );

  if( pNtkTemp == NULL )
  {
    Abc_Print( -1, "Balancing has failed.\n" );
    return NULL;
  }

  //rewriting parameters
  int fUseZeros    = 0;
  int fVerbose     = 0;
  int fVeryVerbose = 0;
  int fPlaceEnable = 0;
  
  //rewriting
  if ( !Abc_NtkRewrite( pNtkTemp, fUpdateLevel, fUseZeros, fVerbose, fVeryVerbose, fPlaceEnable ) )
  {
    Abc_Print( -1, "Rewriting has failed.\n" );
    return NULL;
  }
  
  //rewriting with zero cost
  if ( !Abc_NtkRewrite( pNtkTemp, fUpdateLevel, 1, fVerbose, fVeryVerbose, fPlaceEnable ) )
  {
    Abc_Print( -1, "Rewriting has failed.\n" );
    return NULL;
  }
  
  //balance
  pNtkRes = Abc_NtkBalance( pNtkTemp, fDuplicate, fSelective, fUpdateLevel );

  if( pNtkRes == NULL )
  {
    Abc_Print( -1, "Balancing has failed.\n" );
    return NULL;
  }
  Abc_NtkDelete( pNtkTemp );
  
  //rewriting with zero cost
  if ( !Abc_NtkRewrite( pNtkRes, fUpdateLevel, 1, fVerbose, fVeryVerbose, fPlaceEnable ) )
  {
    Abc_Print( -1, "Rewriting has failed.\n" );
    return NULL;
  }
  
  //balance
  pNtkTemp = Abc_NtkBalance( pNtkRes, fDuplicate, fSelective, fUpdateLevel );

  if( pNtkTemp == NULL )
  {
    Abc_Print( -1, "Balancing has failed.\n" );
    Abc_NtkDelete( pNtkRes );
    return NULL;
  }
  Abc_NtkDelete( pNtkRes );
  
  return pNtkTemp;
}

/**Function*************************************************************

  Synopsis    [read_truth and strash into aig]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Abc_Ntk_t * Truth2Aig( char * pStr )
{
  Abc_Ntk_t * pNtk;
  char * pSopCover;
  
  pSopCover = Abc_SopFromTruthHex(pStr);
  
  if ( pSopCover == NULL || pSopCover[0] == 0 )
  {
    ABC_FREE( pSopCover );
    Abc_Print( -1, "Reading truth table has failed.\n" );
    return NULL;
  }

  pNtk = Abc_NtkCreateWithNode( pSopCover );
  ABC_FREE( pSopCover );
  if ( pNtk == NULL )
  {
    Abc_Print( -1, "Deriving the network has failed.\n" );
    return NULL;
  }
  
  return Abc_NtkStrash( pNtk, 0, 1, 0 );
}


/**Function*************************************************************

  Synopsis    [map AIG into implication logic networks]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Abc_Ntk_t * AigMap2Img( Abc_Ntk_t * pNtk )
{
  Abc_Ntk_t * pNtkRes;
  extern Abc_Ntk_t * Abc_NtkMap( Abc_Ntk_t * pNtk, double DelayTarget, double AreaMulti, double DelayMulti, float LogFan, float Slew, float Gain, int nGatesMin, int fRecovery, int fSwitching, int fSkipFanout, int fUseProfile, int fUseBuffs, int fVerbose );
  
  //check if the network is strashed
  if( !Abc_NtkIsStrash( pNtk ) )
  {
    Abc_Print( -1, " img: This command is only applicable to strashed networks.\n " );
    return NULL;
  }
  
  int fVerbose = 0;

  //read genlib
  /*
   * GATE ZERO      1  Y=CONST0;
   * GATE ONE       1  Y=CONST1;
   * GATE INVX1     1  Y=!A;                        PIN * INV 1 999 1 0 1 0
   * GATE IMPLY     1  Y=(!A+B);                    PIN * INV 1 999 1 0 1 0
   * GATE BUFX1     1  Y=A;                         PIN * NONINV 1 999 1 0 1 0 */
  
  //read_genlib imply.genlib
  Mio_Library_t * pLib = NULL;

  pLib = Mio_LibraryRead( "imply.genlib", NULL, NULL, 0 ); //map/mio/mio.h
  
  if ( pLib == NULL )
  {
    printf( "Reading genlib library has failed.\n" );
    return NULL;
  }

  if ( fVerbose )
  {
    printf( "Entered genlib library with %d gates from file \"%s\".\n", Mio_LibraryReadGateNum(pLib), "imply.genlib" );
  }
  
  // prepare libraries
  Mio_UpdateGenlib( pLib );

  //map
  // set defaults
  double  DelayTarget =-1;
  double  AreaMulti   = 0;
  double  DelayMulti  = 0;
  float   LogFan = 0;
  float   Slew = 0; // choose based on the library
  float   Gain = 250;
  int     nGatesMin = 0;
  int     fRecovery   = 1;
  int     fSwitching  = 0;
  int     fSkipFanout = 0;
  int     fUseProfile = 0;
  int     fUseBuffs   = 0;

  pNtkRes = Abc_NtkMap( pNtk, DelayTarget, AreaMulti, DelayMulti, LogFan, Slew, Gain, nGatesMin, fRecovery, fSwitching, fSkipFanout, fUseProfile, fUseBuffs, fVerbose );
  if ( pNtkRes == NULL )
  {
    Abc_Print( -1, "Mapping has failed.\n" );
    return NULL;
  }

  //get the new network

  return pNtkRes;
}


////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


ABC_NAMESPACE_IMPL_END


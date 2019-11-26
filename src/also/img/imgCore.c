/**CFile****************************************************************

  FileName    [imgCore.c]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [IMG]

  Synopsis    [Implication logic network core]

  Author      [Zhufei Chu]
  
  Affiliation [Ningbo University, China]

***********************************************************************/

#include "base/main/main.h"

ABC_NAMESPACE_IMPL_START

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

int printImgInfo( Abc_Ntk_t * pNtk );

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Img_RunMain( Abc_Frame_t * pAbc ) {
  Abc_Ntk_t * pNtk;
  int result;

  pNtk = Abc_FrameReadNtk( pAbc );

  if( pNtk == NULL ){
    Abc_Print( -1, " Getting the target network has failed.\n " );
    return 0;
  }

  result = printImgInfo( pNtk );

  return result;
}
 
/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/

int printImgInfo( Abc_Ntk_t * pNtk ) {

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

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


ABC_NAMESPACE_IMPL_END


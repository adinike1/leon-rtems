/**
 * @file
 *
 * @ingroup ScoreUserExt
 *
 * @brief User Extension Handler implementation.
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/userext.h>

bool _User_extensions_Thread_create (
  Thread_Control *the_thread
)
{
  Chain_Node              *the_node;
  User_extensions_Control *the_extension;
  bool                     status;

  for ( the_node = _Chain_First( &_User_extensions_List );
        !_Chain_Is_tail( &_User_extensions_List, the_node ) ;
        the_node = the_node->next ) {

    the_extension = (User_extensions_Control *) the_node;

    if ( the_extension->Callouts.thread_create != NULL ) {
      status = (*the_extension->Callouts.thread_create)(
        _Thread_Executing,
        the_thread
      );
      if ( !status )
        return false;
    }
  }

  return true;
}

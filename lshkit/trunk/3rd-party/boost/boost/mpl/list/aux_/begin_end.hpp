
#ifndef BOOST_MPL_LIST_AUX_BEGIN_END_HPP_INCLUDED
#define BOOST_MPL_LIST_AUX_BEGIN_END_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id: begin_end.hpp,v 1.2 2009/02/16 01:51:06 wdong-pku Exp $
// $Date: 2009/02/16 01:51:06 $
// $Revision: 1.2 $

#include <boost/mpl/begin_end_fwd.hpp>
#include <boost/mpl/list/aux_/iterator.hpp>
#include <boost/mpl/list/aux_/tag.hpp>
#include <boost/mpl/list/aux_/item.hpp>

namespace boost { namespace mpl {

template<>
struct begin_impl< aux::list_tag >
{
    template< typename List > struct apply
    {
        typedef l_iter<typename List::type> type;
    };
};

template<>
struct end_impl< aux::list_tag >
{
    template< typename > struct apply
    {
        typedef l_iter<l_end> type;
    };
};

}}

#endif // BOOST_MPL_LIST_AUX_BEGIN_END_HPP_INCLUDED

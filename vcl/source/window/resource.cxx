/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#include <tools/rc.h>

#include <vcl/window.hxx>
#include <vcl/svapp.hxx>

#include "window.h"

namespace vcl {

WinBits Window::ImplInitRes( const ResId& rResId )
{
    GetRes( rResId );

    char* pRes = static_cast<char*>(GetClassRes());
    pRes += 8;
    sal_uInt32 nStyle = (sal_uInt32)GetLongRes( static_cast<void*>(pRes) );
    return nStyle;
}

WindowResHeader Window::ImplLoadResHeader()
{
    WindowResHeader aHeader;

    aHeader.nObjMask = (RscWindowFlags)ReadLongRes();

    // ResourceStyle
    aHeader.nRSStyle = (RSWND)ReadLongRes();
    // WinBits
    ReadLongRes();

    if( aHeader.nObjMask & RscWindowFlags::HelpId )
        aHeader.aHelpId = ReadByteStringRes();

    return aHeader;
}

void Window::ImplLoadRes()
{
    WindowResHeader aHeader = ImplLoadResHeader();

    SetHelpId( aHeader.aHelpId );

    RscWindowFlags nObjMask = aHeader.nObjMask;

    bool  bPos  = false;
    bool  bSize = false;
    Point aPos;
    Size  aSize;

    if ( nObjMask & (RscWindowFlags::XYMapMode | RscWindowFlags::X | RscWindowFlags::Y) )
    {
        // use size as per resource
        MapUnit ePosMap = MapUnit::MapPixel;

        bPos = true;

        if ( nObjMask & RscWindowFlags::XYMapMode )
            ePosMap = (MapUnit)ReadLongRes();
        if ( nObjMask & RscWindowFlags::X )
            aPos.X() = ImplLogicUnitToPixelX( ReadLongRes(), ePosMap );
        if ( nObjMask & RscWindowFlags::Y )
            aPos.Y() = ImplLogicUnitToPixelY( ReadLongRes(), ePosMap );
    }

    if ( nObjMask & (RscWindowFlags::WHMapMode | RscWindowFlags::Width | RscWindowFlags::Height) )
    {
        // use size as per resource
        MapUnit eSizeMap = MapUnit::MapPixel;

        bSize = true;

        if ( nObjMask & RscWindowFlags::WHMapMode )
            eSizeMap = (MapUnit)ReadLongRes();
        if ( nObjMask & RscWindowFlags::Width )
            aSize.Width() = ImplLogicUnitToPixelX( ReadLongRes(), eSizeMap );
        if ( nObjMask & RscWindowFlags::Height )
            aSize.Height() = ImplLogicUnitToPixelY( ReadLongRes(), eSizeMap );
    }

    RSWND nRSStyle = aHeader.nRSStyle;

    // looks bad due to optimization
    if ( nRSStyle & RSWND::CLIENTSIZE )
    {
        if ( bPos )
            SetPosPixel( aPos );
        if ( bSize )
            SetOutputSizePixel( aSize );
    }
    else if ( bPos && bSize )
        SetPosSizePixel( aPos, aSize );
    else if ( bPos )
        SetPosPixel( aPos );
    else if ( bSize )
        SetSizePixel( aSize );

    if ( nRSStyle & RSWND::DISABLED )
        Enable( false );

    if ( nObjMask & RscWindowFlags::Text )
        SetText( ReadStringRes() );
    if ( nObjMask & RscWindowFlags::QuickText )
        SetQuickHelpText( ReadStringRes() );
}

} /* namespace vcl */


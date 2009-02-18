//
//  libavg - Media Playback Engine. 
//  Copyright (C) 2003-2008 Ulrich von Zadow
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Current versions can be found at www.libavg.de
//

#include "ImageNode.h"

#include "SDLDisplayEngine.h"
#include "Player.h"
#include "OGLTiledSurface.h"
#include "NodeDefinition.h"

#include "../base/Logger.h"
#include "../base/ScopeTimer.h"
#include "../base/XMLHelper.h"
#include "../base/Exception.h"

#include "../graphics/Filterfliprgb.h"

#include <Magick++.h>

#include <iostream>
#include <sstream>

using namespace std;

namespace avg {

NodeDefinition ImageNode::createDefinition()
{
    return NodeDefinition("image", Node::buildNode<ImageNode>)
        .extendDefinition(RasterNode::createDefinition())
        .addArg(Arg<string>("href", "", false, offsetof(ImageNode, m_href)));
}

ImageNode::ImageNode(const ArgList& Args, bool bFromXML)
    : m_pImage(new Image("", true))
{
    Args.setMembers(this);
    setHRef(m_href);
}

ImageNode::~ImageNode()
{
}

void ImageNode::setRenderingEngines(DisplayEngine * pDisplayEngine,
        AudioEngine * pAudioEngine)
{
    m_pImage->moveToGPU(dynamic_cast<SDLDisplayEngine*>(pDisplayEngine));
    RasterNode::setRenderingEngines(pDisplayEngine, pAudioEngine);
}

void ImageNode::connect()
{
    RasterNode::connect();
    string sLastFilename = m_pImage->getFilename();
    string sFilename = m_href;
    checkReload();
}

void ImageNode::disconnect()
{
    m_pImage->moveToCPU();
    RasterNode::disconnect();
}

const std::string& ImageNode::getHRef() const
{
    return m_href;
}

void ImageNode::setHRef(const string& href)
{
    m_href = href;
    checkReload();
}

void ImageNode::setBitmap(const Bitmap * pBmp)
{
    if(!pBmp) {
        throw Exception(AVG_ERR_UNSUPPORTED, "setBitmap(): bitmap must not be None!");
    }

    m_pImage = ImagePtr(new Image(pBmp, true));
    m_href = "";
    if (getState() == NS_CANRENDER) {
        m_pImage->moveToGPU(getDisplayEngine());
    }
    IntPoint Size = getMediaSize();
    setViewport(-32767, -32767, Size.x, Size.y);
}

static ProfilingZone RenderProfilingZone("ImageNode::render");

void ImageNode::render(const DRect& Rect)
{
    ScopeTimer Timer(RenderProfilingZone);
    if (m_pImage->getState() == Image::GPU) {
        m_pImage->getTiledSurface()->blt32(getSize(), getEffectiveOpacity(), 
                getBlendMode());
    }
}

IntPoint ImageNode::getMediaSize()
{
    return m_pImage->getSize();
}

void ImageNode::checkReload()
{
    string sLastFilename = m_pImage->getFilename();
    string sFilename = m_href;
    initFilename(sFilename);
    if (sLastFilename != sFilename) {
        try {
            m_pImage = ImagePtr(new Image(sFilename, true));
        } catch (Magick::Exception & ex) {
            m_pImage = ImagePtr(new Image("", true));
            if (getState() == Node::NS_CONNECTED) {
                AVG_TRACE(Logger::ERROR, ex.what());
            } else {
                AVG_TRACE(Logger::MEMORY, ex.what());
            }
        }
        IntPoint Size = getMediaSize();
        setViewport(-32767, -32767, Size.x, Size.y);
    }
    if (getDisplayEngine()) {
        m_pImage->moveToGPU(getDisplayEngine());
    }
}

Bitmap * ImageNode::getBitmap()
{
    return m_pImage->getBitmap();
}

OGLTiledSurface * ImageNode::getSurface()
{
    return m_pImage->getTiledSurface();
}

}

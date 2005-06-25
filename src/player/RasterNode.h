//
// $Id$
// 

#ifndef _RasterNode_H_
#define _RasterNode_H_

#include "Node.h"
#include "IDisplayEngine.h"

#include <string>

namespace avg {

class RasterNode: public Node
{
    public:
        virtual ~RasterNode ();
        void initVisible();
        
        void setAngle(double Angle);
        void setPivotX(double Pivotx);
        void setPivotY(double Pivoty);
        bool setBlendMode(const std::string& sBlendMode);

        // Warping support.
        int getNumVerticesX();
        int getNumVerticesY();
        DPoint getOrigVertexCoord(int x, int y);
        DPoint getWarpedVertexCoord(int x, int y);
        void setWarpedVertexCoord(int x, int y, const DPoint& Vertex);

        OGLSurface * RasterNode::getOGLSurface();
        IDisplayEngine::BlendMode getBlendMode();
        double getAngle();
        virtual std::string getTypeStr ();
        Node * getElementByPos (const DPoint & pos);
        
    protected:
        RasterNode ();
        RasterNode (const xmlNodePtr xmlNode, Container * pParent);
        DPoint getPivot();
        ISurface * getSurface();
 
    private:
        ISurface * m_pSurface;

        double m_Angle;
        bool m_bHasCustomPivot;
        DPoint m_Pivot;
        
        PLPoint m_MaxTileSize;
        std::string m_sBlendMode;
        IDisplayEngine::BlendMode m_BlendMode;
};

}

#endif 


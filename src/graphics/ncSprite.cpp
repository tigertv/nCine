#include <cmath>
#ifdef __ANDROID__
	#include <cstdlib> // for exit()
#endif
#include "ncSprite.h"
#include "ncServiceLocator.h"
#include "ncRenderQueue.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncSprite::ncSprite(ncSceneNode* pParent, ncTexture *pTexture)
	: ncDrawableNode(pParent), m_pTexture(pTexture), m_texRect(0, 0, 0, 0), m_fScaleFactor(1.0f), m_fRotation(0.0f)
{
	Init();
}

ncSprite::ncSprite(ncTexture *pTexture)
	: ncDrawableNode(NULL), m_pTexture(pTexture), m_texRect(0, 0, 0, 0), m_fScaleFactor(1.0f), m_fRotation(0.0f)
{
	Init();
}

ncSprite::ncSprite(ncSceneNode* pParent, ncTexture *pTexture, int iX, int iY)
	: ncDrawableNode(pParent, iX, iY), m_pTexture(pTexture), m_texRect(0, 0, 0, 0), m_fScaleFactor(1.0f), m_fRotation(0.0f)
{
	Init();
}

ncSprite::ncSprite(ncTexture *pTexture, int iX, int iY)
	: ncDrawableNode(NULL, iX, iY), m_pTexture(pTexture), m_texRect(0, 0, 0, 0), m_fScaleFactor(1.0f), m_fRotation(0.0f)
{
	Init();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

ncSprite* ncSprite::FromId(unsigned int uId)
{
	ncObject *pObject = ncServiceLocator::GetIndexer().Object(uId);

	if(pObject)
	{
		if (pObject->Type() == sType())
			return static_cast<ncSprite *>(pObject);
		else // Cannot cast
		{
			ncServiceLocator::GetLogger().Write(ncILogger::LOG_FATAL, "ncSprite::FromId - Object of wrong type");
			exit(-1);
		}
	}
	else // NULL
	{
		ncServiceLocator::GetLogger().Write(ncILogger::LOG_WARN, "ncSprite::FromId - Object not found");
		return NULL;
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ncSprite::Init()
{
	m_eType = SPRITE_TYPE;
	if (m_pTexture)
		SetTexRect(ncRect(0, 0, m_pTexture->Width(), m_pTexture->Height()));
}

void ncSprite::SetVertices()
{
	float leftPos = m_iWidth*m_fScaleFactor*0.5f;
	float rightPos = -m_iWidth*m_fScaleFactor*0.5f;
	float bottomPos = -m_iHeight*m_fScaleFactor*0.5f;
	float topPos = m_iHeight*m_fScaleFactor*0.5f;

	if (m_fRotation > sMinRotation && m_fRotation < 360.0f - sMinRotation)
	{
		float sine = sinf(-m_fRotation * M_PI/180.0f);
		float cosine = cosf(-m_fRotation * M_PI/180.0f);

		m_fVertices[0] = m_absX + leftPos*cosine - bottomPos*sine;	m_fVertices[1] = m_absY + bottomPos*cosine + leftPos*sine;
		m_fVertices[2] = m_absX + leftPos*cosine - topPos*sine;		m_fVertices[3] = m_absY + topPos*cosine + leftPos*sine;
		m_fVertices[4] = m_absX + rightPos*cosine - bottomPos*sine;	m_fVertices[5] = m_absY + bottomPos*cosine + rightPos*sine;

		m_fVertices[6] = m_absX + rightPos*cosine - topPos*sine;	m_fVertices[7] = m_absY + topPos*cosine + rightPos*sine;
	}
	else
	{
		m_fVertices[0] = m_absX + leftPos;	m_fVertices[1] = m_absY + bottomPos;
		m_fVertices[2] = m_absX + leftPos;	m_fVertices[3] = m_absY + topPos;
		m_fVertices[4] = m_absX + rightPos;	m_fVertices[5] = m_absY + bottomPos;

		m_fVertices[6] = m_absX + rightPos;	m_fVertices[7] = m_absY + topPos;
	}
}

void ncSprite::SetTexCoords()
{
	ncPoint texSize = m_pTexture->Size();
	float leftCoord = float(m_texRect.x)/float(texSize.x);
	float rightCoord = float(m_texRect.x+m_texRect.w)/float(texSize.x);
	float bottomCoord = float(m_texRect.y+m_texRect.h)/float(texSize.y);
	float topCoord = float(m_texRect.y)/float(texSize.y);

	m_fTexCoords[0] = leftCoord;	m_fTexCoords[1] = bottomCoord;
	m_fTexCoords[2] = leftCoord;	m_fTexCoords[3] = topCoord;
	m_fTexCoords[4] = rightCoord;	m_fTexCoords[5] = bottomCoord;

	m_fTexCoords[6] = rightCoord;	m_fTexCoords[7] = topCoord;
}

void ncSprite::UpdateRenderCommand()
{
	m_renderCmd.Material().SetTextureGLId(m_pTexture->GLId());
	m_renderCmd.Material().SetColor(m_color.fR(), m_color.fG(), m_color.fB(), m_color.fA());
	m_renderCmd.Transformation().SetPosition(AbsPosition().x, AbsPosition().y);
	SetVertices();
	m_renderCmd.Geometry().SetData(GL_TRIANGLE_STRIP, 0, 4, m_fVertices, m_fTexCoords, NULL);
	m_renderCmd.CalculateSortKey();
}

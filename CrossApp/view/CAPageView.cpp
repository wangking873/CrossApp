//
//  CAPageView.cpp
//  HelloCpp
//
//  Created by 栗元峰 on 14-7-29.
//
//

#include "CAPageView.h"

CAPageView::CAPageView(CAPageViewDirection type)
:m_ePageViewDirection(type)
,m_ePageViewState(CAPageViewNone)
,m_nCurrPage(0)
,m_pPageViewDelegate(NULL)
{

}

CAPageView::~CAPageView()
{
    m_pViews.clear();
}

CAPageView* CAPageView::createWithFrame(const CCRect& rect, CAPageViewDirection type)
{
    CAPageView* view = new CAPageView(type);
    if (view && view->initWithFrame(rect))
    {
        view->autorelease();
        return view;
    }
    CC_SAFE_DELETE(view);
    return NULL;
}

CAPageView* CAPageView::createWithCenter(const CCRect& rect, CAPageViewDirection type)
{
    CAPageView* view = new CAPageView(type);
    if (view && view->initWithCenter(rect))
    {
        view->autorelease();
        return view;
    }
    CC_SAFE_DELETE(view);
    return NULL;
}

bool CAPageView::init()
{
    if (!CAScrollView::init())
    {
        return false;
    }
    
    this->setShowsHorizontalScrollIndicator(false);
    this->setShowsVerticalScrollIndicator(false);

    if (m_ePageViewDirection == CAPageViewDirectionHorizontal)
    {
        this->setTouchMovedListenVertical(false);
        this->setBounceVertical(false);
    }
    else
    {
        this->setTouchMovedListenHorizontal(false);
        this->setBounceHorizontal(false);
    }
    
    return true;
}

void CAPageView::setViews(CAVector<CAView*> vec)
{
    if (!m_pViews.empty())
    {
        for (int i=0; i<m_pViews.size(); i++)
        {
            m_pViews.at(i)->removeFromSuperview();
        }
        m_pViews.clear();
    }
    m_pViews = vec;
    
    if (m_ePageViewDirection == CAPageViewDirectionHorizontal)
    {
        this->setViewSize(CCSize(m_obViewSize.width * m_pViews.size(), m_obViewSize.height));
    }
    else
    {
        this->setViewSize(CCSize(m_obViewSize.width, m_obViewSize.height * m_pViews.size()));
    }
    
    for (int i=0; i<m_pViews.size(); i++)
    {
        CCRect rect = this->getBounds();
        if (m_ePageViewDirection == CAPageViewDirectionHorizontal)
        {
            rect.origin.x = rect.size.width * i;
        }
        else
        {
            rect.origin.y = rect.size.height * i;
        }
        m_pContainer->addSubview(m_pViews.at(i));
        m_pViews.at(i)->setFrame(rect);
    }
}

int CAPageView::getPageCount()
{
    return m_pViews.size();
}

void CAPageView::setCurrPage(int var, bool animated)
{
    var = MIN(var, this->getPageCount() - 1);
    var = MAX(var, 0);
    m_nCurrPage = var;
    if (m_ePageViewDirection == CAPageViewDirectionHorizontal)
    {
        this->setContentOffset(CCPoint(m_nCurrPage * this->getBounds().size.width, 0), animated);
    }
    else
    {
        this->setContentOffset(CCPoint(0, m_nCurrPage * this->getBounds().size.height), animated);
    }
    
    if (m_pPageViewDelegate)
    {
        m_pPageViewDelegate->pageViewDidBeginTurning(this);
    }
}

int CAPageView::getCurrPage()
{
    return m_nCurrPage;
}

void CAPageView::contentOffsetFinish()
{
    if (m_pPageViewDelegate)
    {
        m_pPageViewDelegate->pageViewDidEndTurning(this);
    }
}

void CAPageView::ccTouchEnded(CATouch *pTouch, CAEvent *pEvent)
{
    CAScrollView::ccTouchEnded(pTouch, pEvent);
    
    if (m_ePageViewDirection == CAPageViewDirectionHorizontal)
    {
        float off_x = -m_tInertia.x;
        
        if (off_x > _px(8))
        {
            m_ePageViewState = CAPageViewNext;
        }
        else if (off_x < -_px(8))
        {
            m_ePageViewState = CAPageViewLast;
        }
        else
        {
            m_ePageViewState = CAPageViewNone;
        }
    }
    else
    {
        float off_y = -m_tInertia.y;
        
        if (off_y > _px(8))
        {
            m_ePageViewState = CAPageViewNext;
        }
        else if (off_y < -_px(8))
        {
            m_ePageViewState = CAPageViewLast;
        }
        else
        {
            m_ePageViewState = CAPageViewNone;
        }
    }
    
    int page = this->getCurrPage();
    if (m_ePageViewState == CAPageViewNext)
    {
        page++;
    }
    else if (m_ePageViewState == CAPageViewLast)
    {
        page--;
    }
    else
    {
        if (m_ePageViewDirection == CAPageViewDirectionHorizontal)
        {
            float width = this->getBounds().size.width;
            float off_x = this->getContentOffset().x - this->getCurrPage() * width;
            
            if (off_x > width/2)
            {
                page++;
            }
            else if (off_x < -width/2)
            {
                page--;
            }
        }
        else
        {
            float height = this->getBounds().size.height;
            float off_y = this->getContentOffset().y - this->getCurrPage() * height;
            
            if (off_y > height/2)
            {
                page++;
            }
            else if (off_y < -height/2)
            {
                page--;
            }
        }
    }
    page = MIN(page, this->getPageCount() - 1);
    page = MAX(page, 0);
    this->setCurrPage(page, true);
}




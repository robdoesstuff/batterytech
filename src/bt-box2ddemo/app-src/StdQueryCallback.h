/*
 * StdQueryCallback.h
 *
 *  Created on: Nov 27, 2010
 *      Author: rgreen
 */

#ifndef STDQUERYCALLBACK_H_
#define STDQUERYCALLBACK_H_

#include <bt-box2d/Dynamics/b2WorldCallbacks.h>
#include <bt-box2d/Common/b2Math.h>
#include <bt-box2d/Dynamics/b2Body.h>
#include <bt-box2d/Dynamics/b2Fixture.h>

class StdQueryCallback : public b2QueryCallback
{
public:
	StdQueryCallback(const b2Vec2& point)
	{
		m_point = point;
		m_fixture = NULL;
	}

	bool ReportFixture(b2Fixture* fixture)
	{
		bool inside = fixture->TestPoint(m_point);
		if (inside)
		{
			m_fixture = fixture;

			// We are done, terminate the query.
			return false;
		}

		// Continue the query.
		return true;
	}

	b2Vec2 m_point;
	b2Fixture* m_fixture;
};

#endif /* STDQUERYCALLBACK_H_ */

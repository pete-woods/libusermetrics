/*
 * Copyright (C) 2014 Canonical, Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of version 3 of the GNU Lesser General Public License as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Pete Woods <pete.woods@canonical.com>
 */

#ifndef USERMETRICSOUTPUT_INFOGRAPHICLIST_H_
#define USERMETRICSOUTPUT_INFOGRAPHICLIST_H_

#include <QtCore/QStringList>
#include <QtCore/QStringListModel>

/**
 * @{
 **/

/**
 * @brief The user metrics output library namespace
 **/
namespace UserMetricsOutput {

/**
 * @brief Presentation API for infographics.
 *
 * This class essentially provides a list of paths to
 * infographic image files.
 *
 * Given a user ID, this class watches the corresponding
 *
 **/
class Q_DECL_EXPORT InfographicList: public QStringListModel {
Q_OBJECT

/**
 * @brief The current username selected.
 */
Q_PROPERTY(unsigned int uid READ uid WRITE setUid NOTIFY uidChanged FINAL)

public:
	/**
	 * @brief Get a new instance of InfographicList.
	 */
	static InfographicList *getInstance();

	/**
	 * @brief Destructor.
	 */
	virtual ~InfographicList();

	/**
	 * @brief The current userID selected.
	 */
	virtual unsigned int uid() const = 0;


Q_SIGNALS:
	/**
	 * @brief The user ID has changed
	 *
	 * @param uid
	 */
	void uidChanged(unsigned int uid);

public Q_SLOTS:
	/**
	 * @brief Change the current user ID.
	 *
	 * @param uid
	 *
	 * The data source will change to the first one available
	 * for the given user ID.
	 */
	virtual void setUid(unsigned int uid) = 0;

protected:
	/**
	 * @brief Unusable constructor - this class is pure-virtual.
	 *
	 * @param parent
	 */
	explicit InfographicList(QObject *parent = 0);

	Q_DISABLE_COPY(InfographicList)

};

}

/*@}*/

#endif // USERMETRICSOUTPUT_INFOGRAPHICLIST_H_

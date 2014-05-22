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

#include <QtCore/QObject>

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
 * directory in /var/lib/usermetrics/{userid}/ for changes
 * to SVG files.
 *
 **/
class Q_DECL_EXPORT InfographicList: public QObject {
Q_OBJECT

/**
 * @brief The current user ID selected.
 */
Q_PROPERTY(unsigned int uid READ uid WRITE setUid NOTIFY uidChanged FINAL)

/**
 * @brief The current image path.
 */
Q_PROPERTY(QString path READ path NOTIFY pathChanged FINAL)

public:
	/**
	 * @brief Get a new instance of InfographicList.
	 */
	static InfographicList *getInstance(
			const QString &path = QString("/var/lib/usermetrics/"));

	/**
	 * @brief Destructor.
	 */
	virtual ~InfographicList();

	/**
	 * @brief The current user ID selected.
	 */
	virtual unsigned int uid() const = 0;

	/**
	 * @brief Move to the next infographic image
	 */
	Q_INVOKABLE virtual void next() = 0;

	/**
	 * @brief Access the current infographic image path
	 */
	virtual QString path() const = 0;

Q_SIGNALS:
	/**
	 * @brief The user ID has changed
	 *
	 * @param uid
	 */
	void uidChanged(unsigned int uid);

	/**
     * @brief The current infographic image path has changed
     *
     * @param path
     */
    void pathChanged(const QString &path);

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

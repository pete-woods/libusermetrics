/*
 * Copyright (C) 2013 Canonical, Ltd.
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

#include <libusermetricsoutput/UserMetricsImpl.h>
#include <libusermetricsoutput/UserMetricsStore.h>
#include <libusermetricscommon/DateFactory.h>
#include <libusermetricscommon/Localisation.h>

#include <QtCore/QDate>
#include <QtCore/QString>
#include <QtCore/QVariantList>
#include <QtCore/QDebug>

using namespace UserMetricsOutput;
using namespace UserMetricsCommon;

UserMetricsImpl::UserMetricsImpl(QSharedPointer<DateFactory> dateFactory,
		QSharedPointer<UserMetricsStore> userDataStore,
		QSharedPointer<ColorThemeProvider> colorThemeProvider, QObject *parent) :
		UserMetrics(parent), m_dateFactory(dateFactory), m_userMetricsStore(
				userDataStore), m_colorThemeProvider(colorThemeProvider), m_firstColor(
				new ColorThemeImpl(this)), m_firstMonth(
				new QVariantListModel(this)), m_secondColor(
				new ColorThemeImpl(this)), m_secondMonth(
				new QVariantListModel(this)), m_currentDay(), m_noDataForUser(
				false), m_oldNoDataForUser(false) {
	connect(this, SIGNAL(nextDataSource()), this, SLOT(nextDataSourceSlot()),
			Qt::QueuedConnection);
	connect(this, SIGNAL(readyForDataChange()), this, SLOT(
			readyForDataChangeSlot()), Qt::QueuedConnection);

	// set up a watch for new user data appearing
	connect(m_userMetricsStore.data(),
			SIGNAL(userDataAdded(const QString &, UserDataPtr)), this,
			SLOT(userDataAdded(const QString &, UserDataPtr)));

	setCurrentDay(m_dateFactory->currentDate().day() - 1);
	setUsernameInternal("");
}

UserMetricsImpl::~UserMetricsImpl() {
}

void UserMetricsImpl::setLabel(const QString &label) {
	bool labelChanged = m_label != label;
	m_label = label;
	if (labelChanged) {
		this->labelChanged(m_label);
	}
}

void UserMetricsImpl::setCurrentDay(int currentDay) {
	bool currentDayChanged = m_currentDay != currentDay;
	m_currentDay = currentDay;
	if (currentDayChanged) {
		this->currentDayChanged(m_currentDay);
	}
}

void UserMetricsImpl::setUsername(const QString &username) {
	if (m_username == username) {
		return;
	}

	setUsernameInternal(username);
}

void UserMetricsImpl::userDataAdded(const QString &username,
		UserDataPtr userData) {
	Q_UNUSED(userData);
	if (m_noDataForUser && m_username == username) {
		nextDataSourceSlot();
	}
}

void UserMetricsImpl::dataSetAdded(const QString &dataSourceName) {
	Q_UNUSED(dataSourceName);
	if (m_noDataForUser) {
		nextDataSourceSlot();
	}
}

void UserMetricsImpl::checkForUserData() {
	m_oldNoDataForUser = m_noDataForUser;

	m_userDataIterator = m_userMetricsStore->constFind(m_username);

	// first check to see if there is UserData for this user
	m_noDataForUser = m_userDataIterator == m_userMetricsStore->constEnd();
	if (!m_noDataForUser) {
		// if there is a UserData container
		m_userData = *m_userDataIterator;
		m_dataSetIterator = m_userData->constBegin();

		// now check to see if that container has any data in
		m_noDataForUser = m_dataSetIterator == m_userData->constEnd();
		if (m_noDataForUser) {
			m_watchUser = m_username;
			// set up a watch in-case some data sets are added
			connect(m_userData.data(), SIGNAL(dataSetAdded(const QString &)),
					this, SLOT(dataSetAdded(const QString &)));
		} else {
			m_dataSet = *m_dataSetIterator;
		}
	}
}

void UserMetricsImpl::setUsernameInternal(const QString &username) {
	m_username = username;

	checkForUserData();

	prepareToLoadDataSource();

	usernameChanged(m_username);
}

void UserMetricsImpl::prepareToLoadDataSource() {
	if (!m_dataSet.isNull()) {
		disconnect(m_dataSet.data(), SIGNAL(dataChanged(const QVariantList *)),
				this, SLOT(updateCurrentDataSet(const QVariantList *)));
	}
	if (!m_userData.isNull() && m_watchUser != m_username) {
		disconnect(m_userData.data(), SIGNAL(dataSetAdded(const QString &)),
				this, SLOT(dataSetAdded(const QString &)));
	}
	if (m_oldNoDataForUser && !m_noDataForUser) {
		dataAboutToAppear();
		finishLoadingDataSource();
	} else if (!m_oldNoDataForUser && m_noDataForUser) {
		dataAboutToDisappear();
	} else if (!m_oldNoDataForUser && !m_noDataForUser) {
		dataAboutToChange();
	}
	// we emit no signal if the data has stayed empty
}

void UserMetricsImpl::updateMonth(QVariantListModel &month,
		const int dayOfMonth, const int daysInMonth,
		QVariantList::const_iterator& index,
		const QVariantList::const_iterator& end) {

	// Data for the month
	QVariantList newData;

	// Copy a number of data entries equal to the day of the month it is
	for (int i(0); i < dayOfMonth; ++i) {
		if (index == end) {
			// when we run out of data, pad the remaining days to the
			// start of the month
			newData.prepend(QVariant());
		} else {
			// pop data from the from of the source
			newData.prepend(*index);
			++index;
		}
	}

	// Now fill the end of the month with empty data
	while (newData.size() < daysInMonth) {
		newData.append(QVariant());
	}

	month.setVariantList(newData);
}

void UserMetricsImpl::finishLoadingDataSource() {
	const QDate currentDate(m_dateFactory->currentDate());

	if (m_noDataForUser) {
		QDate secondMonthDate(currentDate.addMonths(-1));

		QVariantList data;
		QVariantList::const_iterator dataIndex(data.begin());
		QVariantList::const_iterator end(data.end());

		updateMonth(*m_firstMonth, 0, currentDate.daysInMonth(), dataIndex,
				end);

		updateMonth(*m_secondMonth, 0, secondMonthDate.daysInMonth(), dataIndex,
				end);

		setLabel("");
	} else {
		updateCurrentDataSet(0);
		connect(m_dataSet.data(), SIGNAL(dataChanged(const QVariantList *)),
				this, SLOT(updateCurrentDataSet(const QVariantList *)));
	}

	setCurrentDay(currentDate.day() - 1);

	if (m_oldNoDataForUser && !m_noDataForUser) {
		dataAppeared();
	} else if (!m_oldNoDataForUser && m_noDataForUser) {
		dataDisappeared();
	} else if (!m_oldNoDataForUser && !m_noDataForUser) {
		dataChanged();
	}
	// we emit no signal if the data has stayed empty
}

void UserMetricsImpl::dataSourceStringsChanged() {
	updateCurrentDataSet(0);
}

void UserMetricsImpl::updateCurrentDataSet(const QVariantList *newData) {
	Q_UNUSED(newData);

	const QDate currentDate(m_dateFactory->currentDate());

	const QDate &lastUpdated(m_dataSet->lastUpdated());
	QDate secondMonthDate(currentDate.addMonths(-1));

	int valuesToCopyForFirstMonth(0);
	int valuesToCopyForSecondMonth(0);

	if (currentDate.year() == lastUpdated.year()
			&& currentDate.month() == lastUpdated.month()) {
		// If the data is for the first month
		valuesToCopyForFirstMonth = lastUpdated.day();
		valuesToCopyForSecondMonth = secondMonthDate.daysInMonth();
	} else if (secondMonthDate.year() == lastUpdated.year()
			&& secondMonthDate.month() == lastUpdated.month()) {
		// If the data is for the second month
		valuesToCopyForSecondMonth = lastUpdated.day();
	} else {
		// the data is out of date
	}

	const QString &dataSourcePath(m_dataSetIterator.key());
	const QVariantList &data(m_dataSet->data());

	QVariantList::const_iterator dataIndex(data.begin());
	QVariantList::const_iterator end(data.end());

	updateMonth(*m_firstMonth, valuesToCopyForFirstMonth,
			currentDate.daysInMonth(), dataIndex, end);

	updateMonth(*m_secondMonth, valuesToCopyForSecondMonth,
			secondMonthDate.daysInMonth(), dataIndex, end);

	DataSourcePtr dataSource(m_userMetricsStore->dataSource(dataSourcePath));
	if (m_dataSourceFormatStringConnection) {
		disconnect(m_dataSourceFormatStringConnection);
	}
	if (m_dataSourceEmptyDataStringConnection) {
		disconnect(m_dataSourceEmptyDataStringConnection);
	}
	m_dataSourceFormatStringConnection = connect(dataSource.data(), SIGNAL(formatStringChanged(const QString &)),
			this, SLOT(dataSourceStringsChanged()));
	m_dataSourceEmptyDataStringConnection = connect(dataSource.data(), SIGNAL(emptyDataStringChanged(const QString &)),
			this, SLOT(dataSourceStringsChanged()));
	if (dataSource.isNull()) {
		qWarning() << _("Data source not found") << " [" << dataSourcePath << "]";
	} else {
		ColorThemePtrPair colorTheme(
				m_colorThemeProvider->getColorTheme(dataSourcePath));
		m_firstColor->setColors(*colorTheme.first);
		m_secondColor->setColors(*colorTheme.second);

		if (data.empty() || currentDate != lastUpdated
				|| m_dataSet->head().isNull()) {
			const QString &emptyDataString = dataSource->emptyDataString();
			if (emptyDataString.isEmpty()) {
				QString empty(_("No data for today"));
				empty.append(" (");
				empty.append(dataSourcePath);
				empty.append(")");
				setLabel(empty);
			} else {
				setLabel(emptyDataString);
			}
		} else {
			QString label;
			if (!dataSource->formatString().isEmpty()) {
				label = dataSource->formatString().arg(
						m_dataSet->head().toString());
			}
			setLabel(label);
		}
	}
}

QString UserMetricsImpl::label() const {
	return m_label;
}

QString UserMetricsImpl::username() const {
	return m_username;
}

ColorTheme * UserMetricsImpl::firstColor() const {
	return m_firstColor.data();
}

ColorTheme * UserMetricsImpl::secondColor() const {
	return m_secondColor.data();
}

QAbstractItemModel * UserMetricsImpl::firstMonth() const {
	return m_firstMonth.data();
}

QAbstractItemModel * UserMetricsImpl::secondMonth() const {
	return m_secondMonth.data();
}

int UserMetricsImpl::currentDay() const {
	return m_currentDay;
}

void UserMetricsImpl::nextDataSourceSlot() {
	if (m_noDataForUser) {
		// check again to see if there's data now
		checkForUserData();
	} else {
		m_oldNoDataForUser = m_noDataForUser;

		++m_dataSetIterator;
		if (m_dataSetIterator == m_userData->constEnd()) {
			m_dataSetIterator = m_userData->constBegin();
		}
		m_dataSet = *m_dataSetIterator;
	}

	prepareToLoadDataSource();
}

void UserMetricsImpl::readyForDataChangeSlot() {
	finishLoadingDataSource();
}


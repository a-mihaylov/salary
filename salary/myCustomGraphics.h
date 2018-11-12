#pragma once
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChart>

QT_CHARTS_BEGIN_NAMESPACE
class QAbstractSeries;
class QPieSlice;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class DrilldownChart : public QChart {
  Q_OBJECT
public:
  explicit DrilldownChart(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0);
  ~DrilldownChart();
  void changeSeries(QAbstractSeries *series);

  public Q_SLOTS:
  void handleSliceClicked(QPieSlice *slice);

private:
  QAbstractSeries *m_currentSeries;
};

// ==============================================================================

class DrilldownSlice : public QPieSlice {
  Q_OBJECT

public:
  DrilldownSlice(qreal value, QString prefix, QAbstractSeries *drilldownSeries);
  virtual ~DrilldownSlice();
  QAbstractSeries *drilldownSeries() const;

  public Q_SLOTS:
  void updateLabel();
  void showHighlight(bool show);

private:
  QAbstractSeries *m_drilldownSeries;
  QString m_prefix;
};
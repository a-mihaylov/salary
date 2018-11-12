#include "myCustomGraphics.h"

DrilldownChart::DrilldownChart(QGraphicsItem *parent, Qt::WindowFlags wFlags)
  : QChart(QChart::ChartTypeCartesian, parent, wFlags),
  m_currentSeries(0) {

}

DrilldownChart::~DrilldownChart() {

}

void DrilldownChart::changeSeries(QAbstractSeries *series) {
  if (m_currentSeries) {
    removeSeries(m_currentSeries);
  }
  m_currentSeries = series;
  addSeries(series);
  setTitle(series->name());
}

void DrilldownChart::handleSliceClicked(QPieSlice *slice) {
  DrilldownSlice *drilldownSlice = static_cast<DrilldownSlice *>(slice);
  changeSeries(drilldownSlice->drilldownSeries());
}

// ==============================================================================

DrilldownSlice::DrilldownSlice(qreal value, QString prefix, QAbstractSeries *drilldownSeries)
  : m_drilldownSeries(drilldownSeries), m_prefix(prefix) {
  setValue(value);
  updateLabel();
  setLabelFont(QFont("Arial", 8));
  connect(this, &DrilldownSlice::percentageChanged, this, &DrilldownSlice::updateLabel);
  connect(this, &DrilldownSlice::hovered, this, &DrilldownSlice::showHighlight);
}

DrilldownSlice::~DrilldownSlice() {

}

QAbstractSeries *DrilldownSlice::drilldownSeries() const {
  return m_drilldownSeries;
}

void DrilldownSlice::updateLabel() {
  QString label = m_prefix;
  label += " - ";
  label += QString::number(this->value());
  label += QString::fromWCharArray(L" ₽, ");
  label += QString::number(this->percentage() * 100, 'f', 1);
  label += "%";
  setLabel(label);
}

void DrilldownSlice::showHighlight(bool show) {
  setLabelVisible(show);
  setExploded(show);
}
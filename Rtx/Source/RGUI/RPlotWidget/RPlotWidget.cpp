#include "RPlotWidget.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QMouseEvent>
#include <QSvgGenerator>



RPlotWidget::RPlotWidget(QWidget *parent)
	: QCustomPlot(parent)//QwtPlot(parent)
{
	this->setOpenGl(false);
	this->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
		QCP::iSelectLegend | QCP::iSelectPlottables);
	this->axisRect()->setupFullAxesBox();
	QList<QCPAxis*> axisrect;
	axisrect << this->xAxis << this->yAxis << this->yAxis2;
	this->axisRect()->setRangeZoomAxes(axisrect);
	this->yAxis2->setVisible(false);

	plotShared = QSharedPointer<QCustomPlot>(this, &QObject::deleteLater);

	//Setting the Legend to be placed right-hand-side of the Plot
	subLayout = new QCPLayoutGrid;
	dummyElement = new QCPLayoutElement;
	dummyElement2 = new QCPLayoutElement;
	this->plotLayout()->addElement(0, 1, subLayout);
	subLayout->addElement(0, 0, dummyElement); // add dummy element above legend
	subLayout->addElement(1, 0, this->legend); // add legend
	subLayout->addElement(2, 0, dummyElement); // add dummy element below legend
	subLayout->setRowStretchFactor(1, 0.1); // make legend cell (in row 0) take up as little vertical space as possible
	this->plotLayout()->setColumnStretchFactor(1, 0.01);
	this->legend->setVisible(true);
	QSize temp = this->legend->iconSize();
	int padding = this->legend->iconTextPadding();
	this->legend->setIconTextPadding(4);
	this->legend->setIconSize(QSize(10,14));
	temp = this->legend->iconSize();
	//this->legend->setIconSize();
	// connect some interaction slots:
	connect(this, SIGNAL(axisDoubleClick(QCPAxis*, QCPAxis::SelectablePart, QMouseEvent*)), this, SLOT(axisLabelDoubleClick(QCPAxis*, QCPAxis::SelectablePart)));
	connect(this, SIGNAL(legendDoubleClick(QCPLegend*, QCPAbstractLegendItem*, QMouseEvent*)), this, SLOT(legendDoubleClick(QCPLegend*, QCPAbstractLegendItem*)));
	//connect(title, SIGNAL(doubleClicked(QMouseEvent*)), this, SLOT(titleDoubleClick(QMouseEvent*)));


	connect(this, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));
	connect(this, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));

	// setup policy and connect slot for context menu popup:
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));

	bottomLogaxisbool = false;
	topLogaxisbool = false;
	leftLogaxisbool = false;
	rightLogaxisbool = false;
	Legendbool = true;

	title = 0;

	//update();


}

RPlotWidget::~RPlotWidget()
{

}


void RPlotWidget::mouseWheel()
{

}

void RPlotWidget::mousePress()
{
	this->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
}

void RPlotWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
	Qt::MouseButton button = event->button();
	Qt::KeyboardModifiers modifiers = event->modifiers();
	if (button == Qt::LeftButton) {
		if (modifiers.testFlag(Qt::NoModifier)) {
			resetZoom();
		} else if (modifiers.testFlag(Qt::ShiftModifier)) {

#ifdef Q_OS_WIN32

#endif // Q_OS_WIN32

		}
	}
}

void RPlotWidget::titleDoubleClick(QMouseEvent* event)
{
	Q_UNUSED(event)
		if (QCPTextElement *title = qobject_cast<QCPTextElement*>(sender()))
		{
			// Set the plot title by double clicking on it
			bool ok;
			QString newTitle = QInputDialog::getText(this, "QCustomPlot example", "New plot title:", QLineEdit::Normal, title->text(), &ok);
			if (ok)
			{
				title->setText(newTitle);
				this->replot();
			}
		}
}

void RPlotWidget::axisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part)
{
	// Set an axis label by double clicking on it
	if (part == QCPAxis::spAxisLabel) // only react when the actual axis label is clicked, not tick label or axis backbone
	{
		bool ok;
		QString newLabel = QInputDialog::getText(this, "QCustomPlot example", "New axis label:", QLineEdit::Normal, axis->label(), &ok);
		if (ok)
		{
			axis->setLabel(newLabel);
			this->replot();
		}
	}
}

void RPlotWidget::legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item)
{
	// Rename a graph by double clicking on its legend item
	Q_UNUSED(legend)
		if (item) // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
		{
			QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem*>(item);
			bool ok;
			QString newName = QInputDialog::getText(this, "QCustomPlot example", "New graph name:", QLineEdit::Normal, plItem->plottable()->name(), &ok);
			if (ok)
			{
				plItem->plottable()->setName(newName);
				this->replot();
			}
		}
}

void RPlotWidget::contextMenuRequest(QPoint pos)
{

	QMenu *menu = new QMenu(this);
	bool ok;
	menu->setAttribute(Qt::WA_DeleteOnClose);

	if (this->legend && this->legend->selectTest(pos, false) >= 0) // context menu on legend requested
	{

		menu->addAction("Move to top left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop | Qt::AlignLeft));
		menu->addAction("Move to top center", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop | Qt::AlignHCenter));
		menu->addAction("Move to top right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop | Qt::AlignRight));
		menu->addAction("Move to bottom right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom | Qt::AlignRight));
		menu->addAction("Move to bottom left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom | Qt::AlignLeft));
	}
	else  // general context menu on graphs requested
	{
		resetZoomAction = new QAction(tr("Reset Zoom"), this);
		resetZoomAction->setStatusTip(tr("Resets the magnification of the diagram."));
		resetZoomAction->setIcon(QIcon(":/RImages/resetzoom.png"));
		ok = connect(resetZoomAction, SIGNAL(triggered()), this, SLOT(resetZoom()));
		menu->addAction(resetZoomAction);
		menu->addSeparator();


		saveDiagramTextAction = new QAction(tr("Save &Data to File"), this);
		saveDiagramTextAction->setStatusTip(tr("Saves the diagram data in a file."));
		saveDiagramTextAction->setIcon(QIcon(":/RImages/saveas.png"));
		ok = connect(saveDiagramTextAction, SIGNAL(triggered()), this, SLOT(saveDiagramToTextFile()));
		menu->addAction(saveDiagramTextAction);
		menu->addSeparator();

		saveDiagramVectorAction = new QAction(tr("Save Diagram As &PDF/PS"), this);
		saveDiagramVectorAction->setStatusTip(tr("Saves the diagram in a PDF or PS file."));
		saveDiagramVectorAction->setIcon(QIcon(":/RImages/pdf.png"));
		ok = connect(saveDiagramVectorAction, SIGNAL(triggered()), this, SLOT(saveDiagramToVectorFile()));
		menu->addAction(saveDiagramVectorAction);

		saveDiagramPixelAction = new QAction(tr("Save Diagram As &Image"), this);
		saveDiagramPixelAction->setStatusTip(tr("Saves the diagram in an image file."));
		saveDiagramPixelAction->setIcon(QIcon(":/RImages/image.png"));
		ok = connect(saveDiagramPixelAction, SIGNAL(triggered()), this, SLOT(saveDiagramToPixelFile()));
		menu->addAction(saveDiagramPixelAction);
		menu->addSeparator();

		printDiagramAction = new QAction(tr("&Print Preview"), this);
		printDiagramAction->setStatusTip(tr("For Printing the diagram."));
		printDiagramAction->setIcon(QIcon(":/RImages/print.png"));
		ok = connect(printDiagramAction, SIGNAL(triggered()), this, SLOT(printPreview()));
		menu->addAction(printDiagramAction);
		menu->addSeparator();

		menu->addAction("Add/Edit Title", this, SLOT(addTitle()));
	}

	menu->addSeparator();


#pragma region BottomAxis

	bottomAxisMenu = menu->addMenu(tr("&Bottom Axis"));

	bottomAxisLabelAction = new QAction(tr("&Label"), this);
	bottomAxisLabelAction->setStatusTip(tr("Sets the label of the bottom axis."));
	ok = connect(bottomAxisLabelAction, SIGNAL(triggered()), this, SLOT(setBottomAxisLabel()));
	bottomAxisMenu->addAction(bottomAxisLabelAction);

	bottomAxisMinAction = new QAction(tr("&Minimum"), this);
	bottomAxisMinAction->setStatusTip(tr("Sets the minimum of the bottom axis."));
	ok = connect(bottomAxisMinAction, SIGNAL(triggered()), this, SLOT(setBottomAxisMin()));
	bottomAxisMenu->addAction(bottomAxisMinAction);

	bottomAxisMaxAction = new QAction(tr("&Maximum"), this);
	bottomAxisMaxAction->setStatusTip(tr("Sets the maximum of the bottom axis."));
	ok = connect(bottomAxisMaxAction, SIGNAL(triggered()), this, SLOT(setBottomAxisMax()));
	bottomAxisMenu->addAction(bottomAxisMaxAction);

	bottomAxisLogAction = new QAction(tr("&Logarithmic"), this);
	bottomAxisLogAction->setStatusTip(tr("Toggles the scale of the bottom axis between linear and logarithmic."));
	ok = connect(bottomAxisLogAction, SIGNAL(toggled(bool)), this, SLOT(setBottomAxisLog(bool)));
	bottomAxisLogAction->setCheckable(true);
	bottomAxisLogAction->setChecked(bottomLogaxisbool);
	bottomAxisMenu->addAction(bottomAxisLogAction);
#pragma endregion BottomAxis

#pragma region LeftAxis

	leftAxisMenu = menu->addMenu(tr("&Left Axis"));

	leftAxisLabelAction = new QAction(tr("&Label"), this);
	leftAxisLabelAction->setStatusTip(tr("Sets the label of the left axis."));
	//leftAxisLabelAction->setIcon(QIcon(":/RImages/print.png"));
	ok = connect(leftAxisLabelAction, SIGNAL(triggered()), this, SLOT(setLeftAxisLabel()));
	leftAxisMenu->addAction(leftAxisLabelAction);

	leftAxisMinAction = new QAction(tr("&Minimum"), this);
	leftAxisMinAction->setStatusTip(tr("Sets the minimum of the left axis."));
	//leftAxisMinAction->setIcon(QIcon(":/RImages/print.png"));
	ok = connect(leftAxisMinAction, SIGNAL(triggered()), this, SLOT(setLeftAxisMin()));
	leftAxisMenu->addAction(leftAxisMinAction);

	leftAxisMaxAction = new QAction(tr("&Maximum"), this);
	leftAxisMaxAction->setStatusTip(tr("Sets the maximum of the left axis."));
	//leftAxisMaxAction->setIcon(QIcon(":/RImages/print.png"));
	ok = connect(leftAxisMaxAction, SIGNAL(triggered()), this, SLOT(setLeftAxisMax()));
	leftAxisMenu->addAction(leftAxisMaxAction);

	leftAxisLogAction = new QAction(tr("&Logarithmic"), this);
	leftAxisLogAction->setStatusTip(tr("Toggles the scale of the left axis between linear and logarithmic."));
	//leftAxisLogAction->setIcon(QIcon(":/RImages/print.png"));
	ok = connect(leftAxisLogAction, SIGNAL(toggled(bool)), this, SLOT(setLeftAxisLog(bool)));
	leftAxisLogAction->setCheckable(true);
	leftAxisLogAction->setChecked(leftLogaxisbool);
	leftAxisMenu->addAction(leftAxisLogAction);
#pragma endregion LeftAxis

	if (this->xAxis2->visible())
	{
		topAxisMenu = menu->addMenu(tr("&Top Axis"));

		topAxisLabelAction = new QAction(tr("&Label"), this);
		topAxisLabelAction->setStatusTip(tr("Sets the label of the top axis."));
		//topAxisLabelAction->setIcon(QIcon(":/RImages/print.png"));
		ok = connect(topAxisLabelAction, SIGNAL(triggered()), this, SLOT(setTopAxisLabel()));
		topAxisMenu->addAction(topAxisLabelAction);

		topAxisMinAction = new QAction(tr("&Minimum"), this);
		topAxisMinAction->setStatusTip(tr("Sets the minimum of the top axis."));
		//topAxisMinAction->setIcon(QIcon(":/RImages/print.png"));
		ok = connect(topAxisMinAction, SIGNAL(triggered()), this, SLOT(setTopAxisMin()));
		topAxisMenu->addAction(topAxisMinAction);

		topAxisMaxAction = new QAction(tr("&Maximum"), this);
		topAxisMaxAction->setStatusTip(tr("Sets the maximum of the top axis."));
		//topAxisMaxAction->setIcon(QIcon(":/RImages/print.png"));
		ok = connect(topAxisMaxAction, SIGNAL(triggered()), this, SLOT(setTopAxisMax()));
		topAxisMenu->addAction(topAxisMaxAction);

		topAxisLogAction = new QAction(tr("&Logarithmic"), this);
		topAxisLogAction->setStatusTip(tr("Toggles the scale of the top axis between linear and logarithmic."));
		//topAxisLogAction->setIcon(QIcon(":/RImages/print.png"));
		ok = connect(topAxisLogAction, SIGNAL(toggled(bool)), this, SLOT(setTopAxisLog(bool)));
		topAxisLogAction->setCheckable(true);
		topAxisLogAction->setChecked(topLogaxisbool);
		topAxisMenu->addAction(topAxisLogAction);
	}

	if (this->yAxis2->visible())
	{
		rightAxisMenu = menu->addMenu(tr("&Right Axis"));

		rightAxisLabelAction = new QAction(tr("&Label"), this);
		rightAxisLabelAction->setStatusTip(tr("Sets the label of the right axis."));
		//rightAxisLabelAction->setIcon(QIcon(":/RImages/print.png"));
		ok = connect(rightAxisLabelAction, SIGNAL(triggered()), this, SLOT(setRightAxisLabel()));
		rightAxisMenu->addAction(rightAxisLabelAction);

		rightAxisMinAction = new QAction(tr("&Minimum"), this);
		rightAxisMinAction->setStatusTip(tr("Sets the minimum of the right axis."));
		//rightAxisMinAction->setIcon(QIcon(":/RImages/print.png"));
		ok = connect(rightAxisMinAction, SIGNAL(triggered()), this, SLOT(setRightAxisMin()));
		rightAxisMenu->addAction(rightAxisMinAction);

		rightAxisMaxAction = new QAction(tr("&Maximum"), this);
		rightAxisMaxAction->setStatusTip(tr("Sets the maximum of the right axis."));
		//rightAxisMaxAction->setIcon(QIcon(":/RImages/print.png"));
		ok = connect(rightAxisMaxAction, SIGNAL(triggered()), this, SLOT(setRightAxisMax()));
		rightAxisMenu->addAction(rightAxisMaxAction);

		rightAxisLogAction = new QAction(tr("&Logarithmic"), this);
		rightAxisLogAction->setStatusTip(tr("Toggles the scale of the right axis between linear and logarithmic."));
		//rightAxisLogAction->setIcon(QIcon(":/RImages/print.png"));
		ok = connect(rightAxisLogAction, SIGNAL(toggled(bool)), this, SLOT(setRightAxisLog(bool)));
		rightAxisLogAction->setCheckable(true);
		rightAxisMenu->addAction(rightAxisLogAction);
	}
	menu->addSeparator();

	showLegendAction = new QAction(tr("Show &Legend"), this);
	showLegendAction->setStatusTip(tr("Shows/hides the legend."));
	ok = connect(showLegendAction, SIGNAL(toggled(bool)), this, SLOT(showLegend(bool)));
	showLegendAction->setCheckable(true);
	showLegendAction->setChecked(Legendbool);
	menu->addAction(showLegendAction);

	menu->popup(this->mapToGlobal(pos));
}

void RPlotWidget::showLegend(bool checked)
{
	Legendbool = checked;

	if (checked) {

		this->legend->setVisible(true);
		this->subLayout->setVisible(true);
		this->plotLayout()->setColumnStretchFactor(1, 0.01);

	} else {
		this->legend->setVisible(false);
		this->subLayout->setVisible(false);
		this->subLayout->setColumnStretchFactor(0.01, 0.01);

	}
	replot();
}



void RPlotWidget::resetZoom()
{
	this->xAxis->rescale(true);
	this->yAxis->rescale(true);
	this->yAxis2->rescale(true);
	replot();
}

void RPlotWidget::moveLegend()
{
	if (QAction* contextAction = qobject_cast<QAction*>(sender())) // make sure this slot is really called by a context menu action, so it carries the data we need
	{
		bool ok;
		int dataInt = contextAction->data().toInt(&ok);
		if (ok)
		{
			this->axisRect()->insetLayout()->setInsetAlignment(0, (Qt::Alignment)dataInt);
			this->replot();
		}
	}
}

void RPlotWidget::addTitle()
{
	bool ok;
	QString newName = QInputDialog::getText(this, "QCustomPlot example", "New graph name:", QLineEdit::Normal, "Untitled Graph", &ok);
	if (ok)
	{
		if (title == NULL )
		{
			title = new QCPTextElement(this, newName, QFont("sans", 17, QFont::Bold));
			this->plotLayout()->insertRow(0);
			this->plotLayout()->addElement(0, 0, title);
			connect(title, SIGNAL(doubleClicked(QMouseEvent*)), this, SLOT(titleDoubleClick(QMouseEvent*)));
		}
		else
		{
			title->setText(newName);
		}

		this->replot();
	}

}

void RPlotWidget::saveDiagramToTextFile()
{

	QList<QWidget *> childList = findChildren<QWidget *>();

	if (this->graphCount() == 0) {
		return;
	}

	QString fileName = QFileDialog::getSaveFileName(this, tr("Save Text File"), QDir::currentPath() + "/" + windowTitle(), "Text Files (*.txt)");
	if (fileName.isEmpty()) {
		return;
	}

	if (QFileInfo(fileName).suffix().toLower() != QString("txt").toLower()) {
		fileName += ".txt";
	}

	// Open the file
	QFile file(fileName);
	if (!file.open(QFile::WriteOnly | QFile::Text)) {
		// Error
		QMessageBox::critical(this, tr("Error"), QString("Could not create the file %1.").arg(fileName), QMessageBox::Ok | QMessageBox::Default);
		return;
	}


	for (int i = 0; i < this->graphCount(); i++) {

		int dataCount = this->graph(i)->dataCount();

		for (int j = 0; j < dataCount; j++) {
			double x = this->graph(i)->dataMainKey(j);
			double y = this->graph(i)->dataMainValue(j);

			QString tempString = QString("%1\t%2\n").arg(x, 0, 'g', 8).arg(y, 0, 'g', 8);
			if (this->graphCount() > 1) {
				if (j == 0) {
					tempString = this->graph(i)->name() + "\n" + tempString;
					if (i > 0) {
						tempString = "\n" + tempString;
					}
				}
			}

			QByteArray byteArray;
			byteArray.append(tempString);
			file.write(byteArray);
			file.flush();
		}
		if (i < this->graphCount() - 1) {
			QByteArray byteArray = "\n\n";
			file.write(byteArray);
			file.flush();
		}
	}

	file.close();
}


void RPlotWidget::saveDiagramToVectorFile()
{
	QString selectedFilter;
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save PDF/PS File"), QDir::currentPath() + "/" + windowTitle(), "PDF Files (*.pdf);;PostScript Files (*.ps)", &selectedFilter);
	if (fileName.isEmpty()) {
		return;
	}
	QPrinter printer;
	printer.setOutputFileName(fileName);
	renderPlot(&printer);
}

void RPlotWidget::printPreview()
{
	QPrinter printer;
	QPrintPreviewDialog previewDialog(&printer, this);
	connect(&previewDialog, SIGNAL(paintRequested(QPrinter*)), SLOT(renderPlot(QPrinter*)));
	previewDialog.exec();
}

void RPlotWidget::renderPlot(QPrinter *printer)
{
	printer->setPageSize(QPrinter::A4);
	QCPPainter painter(printer);
	QRectF pageRect = printer->pageRect(QPrinter::DevicePixel);

	int plotWidth = this->viewport().width();
	int plotHeight = this->viewport().height();
	double scale = pageRect.width() / (double)plotWidth;

	painter.setMode(QCPPainter::pmVectorized);
	painter.setMode(QCPPainter::pmNoCaching);
	painter.setMode(QCPPainter::pmNonCosmetic); // comment this out if you want cosmetic thin lines (always 1 pixel thick independent of pdf zoom level)

	painter.scale(scale, scale);
	this->toPainter(&painter, plotWidth, plotHeight);
}

void RPlotWidget::saveDiagramToPixelFile()
{
	int scale = 1;
	int width = this->width() * scale;
	int height = this->height() * scale;
	QPixmap pixmap(width, height);
	pixmap.fill(Qt::white); // Qt::transparent ?

	QString selectedFilter;
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image File"), QDir::currentPath() + "/" + windowTitle(), "PNG Files (*.png);;JPG Files (*.jpg);;BMP Files (*.bmp);;PPM Files (*.PPM);;XBM Files (*.xbm);;XPM Files (*.xpm)", &selectedFilter);
	if (fileName.isEmpty()) {
		return;
	}

	QString suffix = QFileInfo(fileName).suffix();

	QList<QByteArray> formats = QImageWriter::supportedImageFormats();


	for (int i = 0; i < formats.count(); i++)
	{
		QByteArray format = formats[i];
		format;
	}

	if (suffix == "png")
		this->savePng(fileName);
	else if (suffix == "bmp")
	{
		this->saveBmp(fileName);
	}
	else if (suffix == "jpg")
	{
		this->saveJpg(fileName);
	}
	else if (suffix == "pdf")
	{
		this->savePdf(fileName);
	}
	else if (suffix == "ppm")
	{
		this->saveRastered(fileName, width, height, scale, "ppm", -1, 96);
	}
	else if (suffix == "xbm")
	{
		this->saveRastered(fileName, width, height, scale, "xbm", -1, 96);
	}
	else if (suffix == "xpm")
	{
		this->saveRastered(fileName, width, height, scale, "xpm", -1, 96);
	}
}

void RPlotWidget::setBottomAxisLabel()
{

	bool ok;
	QString label = QInputDialog::getText(0, tr("Axis Label"), tr("Enter the label"), QLineEdit::Normal, "", &ok);
	if (ok) {
		this->xAxis->setLabel(label);
	}
	this->replot();




}

void RPlotWidget::setBottomAxisMin()
{

	bool ok;
	QString text = QInputDialog::getText(0, tr("Axis Minimum"), tr("Enter the minimum"), QLineEdit::Normal, "", &ok);
	double value;
	if (ok && !text.isEmpty()) {
		value = text.toDouble(&ok);
	}
	if (ok) {
		this->xAxis->setRange(value, this->xAxis->range().upper);
		replot();
	}

}

void RPlotWidget::setBottomAxisMax()
{

	bool ok;
	QString text = QInputDialog::getText(0, tr("Axis Maximum"), tr("Enter the maximum"), QLineEdit::Normal, "", &ok);
	double value;
	if (ok && !text.isEmpty()) {
		value = text.toDouble(&ok);
	}
	if (ok) {
		this->xAxis->setRange(this->xAxis->range().lower , value);
		replot();
	}


}

void RPlotWidget::setBottomAxisLog(bool checked)
{
	bottomLogaxisbool = checked;

	this->xAxis->grid()->setSubGridVisible(checked);

	if (checked)
	{
		this->xAxis->setScaleType(QCPAxis::stLogarithmic);
	}
	else
	{
		this->xAxis->setScaleType(QCPAxis::stLinear);
	}

	this->replot();


}

void RPlotWidget::setTopAxisLabel()
{
	bool ok;
	QString label = QInputDialog::getText(0, tr("Axis Label"), tr("Enter the label"), QLineEdit::Normal, "", &ok);
	if (ok) {
		this->xAxis2->setLabel(label);
	}
	this->replot();

	//setAxisLabel(QwtPlot::xTop);
}

void RPlotWidget::setTopAxisMin()
{
	bool ok;
	QString text = QInputDialog::getText(0, tr("Axis Minimum"), tr("Enter the minimum"), QLineEdit::Normal, "", &ok);
	double value;
	if (ok && !text.isEmpty()) {
		value = text.toDouble(&ok);
	}
	if (ok) {
		this->xAxis2->setRange(value, this->xAxis2->range().upper);
		replot();
	}

	//setAxisMin(QwtPlot::xTop);
}

void RPlotWidget::setTopAxisMax()
{
	bool ok;
	QString text = QInputDialog::getText(0, tr("Axis Maximum"), tr("Enter the maximum"), QLineEdit::Normal, "", &ok);
	double value;
	if (ok && !text.isEmpty()) {
		value = text.toDouble(&ok);
	}
	if (ok) {
		this->xAxis2->setRange(this->xAxis2->range().lower, value);
		replot();
	}

	//setAxisMax(QwtPlot::xTop);
}

void RPlotWidget::setTopAxisLog(bool checked)
{

	topLogaxisbool = checked;

	this->xAxis2->grid()->setSubGridVisible(checked);

	if (checked)
	{
		this->xAxis2->setScaleType(QCPAxis::stLogarithmic);
	}
	else
	{
		this->xAxis2->setScaleType(QCPAxis::stLinear);
	}

	this->replot();

	//setAxisLog(QwtPlot::xTop, checked);
}

void RPlotWidget::setLeftAxisLabel()
{
	bool ok;
	QString label = QInputDialog::getText(0, tr("Axis Label"), tr("Enter the label"), QLineEdit::Normal, "", &ok);
	if (ok) {
		this->yAxis->setLabel(label);
	}
	this->replot();

	//setAxisLabel(QwtPlot::yLeft);
}

void RPlotWidget::setLeftAxisMin()
{
	bool ok;
	QString text = QInputDialog::getText(0, tr("Axis Minimum"), tr("Enter the minimum"), QLineEdit::Normal, "", &ok);
	double value;
	if (ok && !text.isEmpty()) {
		value = text.toDouble(&ok);
	}
	if (ok) {
		this->yAxis->setRange(value, this->yAxis->range().upper);
		replot();
	}

	//setAxisMin(QwtPlot::yLeft);
}

void RPlotWidget::setLeftAxisMax()
{
	bool ok;
	QString text = QInputDialog::getText(0, tr("Axis Maximum"), tr("Enter the maximum"), QLineEdit::Normal, "", &ok);
	double value;
	if (ok && !text.isEmpty()) {
		value = text.toDouble(&ok);
	}
	if (ok) {
		this->yAxis->setRange(this->yAxis->range().lower, value);
		replot();
	}

	//setAxisMax(QwtPlot::yLeft);
}

void RPlotWidget::setLeftAxisLog(bool checked)
{
	leftLogaxisbool = checked;

	this->yAxis->grid()->setSubGridVisible(checked);

	if (checked)
	{
		this->yAxis->setScaleType(QCPAxis::stLogarithmic);
	}
	else
	{
		this->yAxis->setScaleType(QCPAxis::stLinear);
	}

	this->replot();

	//setAxisLog(QwtPlot::yLeft, checked);
}

void RPlotWidget::setRightAxisLabel()
{
	bool ok;
	QString label = QInputDialog::getText(0, tr("Axis Label"), tr("Enter the label"), QLineEdit::Normal, "", &ok);
	if (ok) {
		this->yAxis2->setLabel(label);
	}
	this->replot();

	//setAxisLabel(QwtPlot::yRight);
}

void RPlotWidget::setRightAxisMin()
{
	bool ok;
	QString text = QInputDialog::getText(0, tr("Axis Minimum"), tr("Enter the minimum"), QLineEdit::Normal, "", &ok);
	double value;
	if (ok && !text.isEmpty()) {
		value = text.toDouble(&ok);
	}
	if (ok) {
		this->yAxis2->setRange(value, this->yAxis2->range().upper);
		replot();
	}

	//setAxisMin(QwtPlot::yRight);
}

void RPlotWidget::setRightAxisMax()
{
	bool ok;
	QString text = QInputDialog::getText(0, tr("Axis Maximum"), tr("Enter the maximum"), QLineEdit::Normal, "", &ok);
	double value;
	if (ok && !text.isEmpty()) {
		value = text.toDouble(&ok);
	}
	if (ok) {
		this->yAxis2->setRange(this->yAxis2->range().lower, value);
		replot();
	}

	//setAxisMax(QwtPlot::yRight);
}

void RPlotWidget::setRightAxisLog(bool checked)
{
	rightLogaxisbool = checked;

	this->yAxis2->grid()->setSubGridVisible(checked);

	if (checked)
	{
		this->yAxis2->setScaleType(QCPAxis::stLogarithmic);
	}
	else
	{
		this->yAxis2->setScaleType(QCPAxis::stLinear);
	}

	this->replot();
	//setAxisLog(QwtPlot::yRight, checked);
}



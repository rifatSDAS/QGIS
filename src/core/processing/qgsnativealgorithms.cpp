/***************************************************************************
                         qgsnativealgorithms.cpp
                         ---------------------
    begin                : April 2017
    copyright            : (C) 2017 by Nyall Dawson
    email                : nyall dot dawson at gmail dot com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsnativealgorithms.h"
#include "qgsfeatureiterator.h"
#include "qgsprocessingcontext.h"
#include "qgsprocessingfeedback.h"
#include "qgsprocessingutils.h"
#include "qgsvectorlayer.h"
#include "qgsgeometry.h"
#include "qgsgeometryengine.h"
#include "qgswkbtypes.h"

#include <functional>

///@cond PRIVATE

QgsNativeAlgorithms::QgsNativeAlgorithms( QObject *parent )
  : QgsProcessingProvider( parent )
{}

QIcon QgsNativeAlgorithms::icon() const
{
  return QgsApplication::getThemeIcon( QStringLiteral( "/providerQgis.svg" ) );
}

QString QgsNativeAlgorithms::svgIconPath() const
{
  return QgsApplication::iconPath( QStringLiteral( "providerQgis.svg" ) );
}

QString QgsNativeAlgorithms::id() const
{
  return QStringLiteral( "native" );
}

QString QgsNativeAlgorithms::name() const
{
  return tr( "QGIS (native c++)" );
}

bool QgsNativeAlgorithms::supportsNonFileBasedOutput() const
{
  return true;
}

void QgsNativeAlgorithms::loadAlgorithms()
{
  addAlgorithm( new QgsBufferAlgorithm() );
  addAlgorithm( new QgsCentroidAlgorithm() );
  addAlgorithm( new QgsClipAlgorithm() );
  addAlgorithm( new QgsDissolveAlgorithm() );
  addAlgorithm( new QgsCollectAlgorithm() );
  addAlgorithm( new QgsExtractByAttributeAlgorithm() );
  addAlgorithm( new QgsExtractByExpressionAlgorithm() );
  addAlgorithm( new QgsMultipartToSinglepartAlgorithm() );
  addAlgorithm( new QgsSubdivideAlgorithm() );
  addAlgorithm( new QgsTransformAlgorithm() );
  addAlgorithm( new QgsRemoveNullGeometryAlgorithm() );
  addAlgorithm( new QgsBoundingBoxAlgorithm() );
  addAlgorithm( new QgsOrientedMinimumBoundingBoxAlgorithm() );
  addAlgorithm( new QgsMinimumEnclosingCircleAlgorithm() );
  addAlgorithm( new QgsConvexHullAlgorithm() );
  addAlgorithm( new QgsPromoteToMultipartAlgorithm() );
  addAlgorithm( new QgsSelectByLocationAlgorithm() );
  addAlgorithm( new QgsExtractByLocationAlgorithm() );
}

void QgsCentroidAlgorithm::initAlgorithm( const QVariantMap & )
{
  addParameter( new QgsProcessingParameterFeatureSource( QStringLiteral( "INPUT" ), QObject::tr( "Input layer" ) ) );
  addParameter( new QgsProcessingParameterFeatureSink( QStringLiteral( "OUTPUT" ), QObject::tr( "Centroids" ), QgsProcessing::TypeVectorPoint ) );
}

QString QgsCentroidAlgorithm::shortHelpString() const
{
  return QObject::tr( "This algorithm creates a new point layer, with points representing the centroid of the geometries in an input layer.\n\n"
                      "The attributes associated to each point in the output layer are the same ones associated to the original features." );
}

QgsCentroidAlgorithm *QgsCentroidAlgorithm::createInstance() const
{
  return new QgsCentroidAlgorithm();
}

QgsFeature QgsCentroidAlgorithm::processFeature( const QgsFeature &f, QgsProcessingFeedback *feedback )
{
  QgsFeature feature = f;
  if ( feature.hasGeometry() )
  {
    feature.setGeometry( feature.geometry().centroid() );
    if ( !feature.geometry() )
    {
      feedback->pushInfo( QObject::tr( "Error calculating centroid for feature %1" ).arg( feature.id() ) );
    }
  }
  return feature;
}
//
// QgsBufferAlgorithm
//

void QgsBufferAlgorithm::initAlgorithm( const QVariantMap & )
{
  addParameter( new QgsProcessingParameterFeatureSource( QStringLiteral( "INPUT" ), QObject::tr( "Input layer" ) ) );
  addParameter( new QgsProcessingParameterNumber( QStringLiteral( "DISTANCE" ), QObject::tr( "Distance" ), QgsProcessingParameterNumber::Double, 10 ) );
  addParameter( new QgsProcessingParameterNumber( QStringLiteral( "SEGMENTS" ), QObject::tr( "Segments" ), QgsProcessingParameterNumber::Integer, 5, false, 1 ) );

  addParameter( new QgsProcessingParameterEnum( QStringLiteral( "END_CAP_STYLE" ), QObject::tr( "End cap style" ), QStringList() << QObject::tr( "Round" ) << QObject::tr( "Flat" ) << QObject::tr( "Square" ), false ) );
  addParameter( new QgsProcessingParameterEnum( QStringLiteral( "JOIN_STYLE" ), QObject::tr( "Join style" ), QStringList() << QObject::tr( "Round" ) << QObject::tr( "Miter" ) << QObject::tr( "Bevel" ), false ) );
  addParameter( new QgsProcessingParameterNumber( QStringLiteral( "MITER_LIMIT" ), QObject::tr( "Miter limit" ), QgsProcessingParameterNumber::Double, 2, false, 1 ) );

  addParameter( new QgsProcessingParameterBoolean( QStringLiteral( "DISSOLVE" ), QObject::tr( "Dissolve result" ), false ) );
  addParameter( new QgsProcessingParameterFeatureSink( QStringLiteral( "OUTPUT" ), QObject::tr( "Buffered" ), QgsProcessing::TypeVectorPolygon ) );
}

QString QgsBufferAlgorithm::shortHelpString() const
{
  return QObject::tr( "This algorithm computes a buffer area for all the features in an input layer, using a fixed or dynamic distance.\n\n"
                      "The segments parameter controls the number of line segments to use to approximate a quarter circle when creating rounded offsets.\n\n"
                      "The end cap style parameter controls how line endings are handled in the buffer.\n\n"
                      "The join style parameter specifies whether round, miter or beveled joins should be used when offsetting corners in a line.\n\n"
                      "The miter limit parameter is only applicable for miter join styles, and controls the maximum distance from the offset curve to use when creating a mitered join." );
}

QgsBufferAlgorithm *QgsBufferAlgorithm::createInstance() const
{
  return new QgsBufferAlgorithm();
}

QVariantMap QgsBufferAlgorithm::processAlgorithm( const QVariantMap &parameters, QgsProcessingContext &context, QgsProcessingFeedback *feedback )
{
  std::unique_ptr< QgsFeatureSource > source( parameterAsSource( parameters, QStringLiteral( "INPUT" ), context ) );
  if ( !source )
    return QVariantMap();

  QString dest;
  std::unique_ptr< QgsFeatureSink > sink( parameterAsSink( parameters, QStringLiteral( "OUTPUT" ), context, dest, source->fields(), QgsWkbTypes::Polygon, source->sourceCrs() ) );
  if ( !sink )
    return QVariantMap();

  // fixed parameters
  bool dissolve = parameterAsBool( parameters, QStringLiteral( "DISSOLVE" ), context );
  int segments = parameterAsInt( parameters, QStringLiteral( "SEGMENTS" ), context );
  QgsGeometry::EndCapStyle endCapStyle = static_cast< QgsGeometry::EndCapStyle >( 1 + parameterAsInt( parameters, QStringLiteral( "END_CAP_STYLE" ), context ) );
  QgsGeometry::JoinStyle joinStyle = static_cast< QgsGeometry::JoinStyle>( 1 + parameterAsInt( parameters, QStringLiteral( "JOIN_STYLE" ), context ) );
  double miterLimit = parameterAsDouble( parameters, QStringLiteral( "MITER_LIMIT" ), context );
  double bufferDistance = parameterAsDouble( parameters, QStringLiteral( "DISTANCE" ), context );
  bool dynamicBuffer = QgsProcessingParameters::isDynamic( parameters, QStringLiteral( "DISTANCE" ) );
  const QgsProcessingParameterDefinition *distanceParamDef = parameterDefinition( QStringLiteral( "DISTANCE" ) );

  long count = source->featureCount();

  QgsFeature f;
  QgsFeatureIterator it = source->getFeatures();

  double step = count > 0 ? 100.0 / count : 1;
  int current = 0;

  QList< QgsGeometry > bufferedGeometriesForDissolve;
  QgsAttributes dissolveAttrs;

  while ( it.nextFeature( f ) )
  {
    if ( feedback->isCanceled() )
    {
      break;
    }
    if ( dissolveAttrs.isEmpty() )
      dissolveAttrs = f.attributes();

    QgsFeature out = f;
    if ( out.hasGeometry() )
    {
      if ( dynamicBuffer )
      {
        context.expressionContext().setFeature( f );
        bufferDistance = QgsProcessingParameters::parameterAsDouble( distanceParamDef, parameters, context );
      }

      QgsGeometry outputGeometry = f.geometry().buffer( bufferDistance, segments, endCapStyle, joinStyle, miterLimit );
      if ( !outputGeometry )
      {
        QgsMessageLog::logMessage( QObject::tr( "Error calculating buffer for feature %1" ).arg( f.id() ), QObject::tr( "Processing" ), QgsMessageLog::WARNING );
      }
      if ( dissolve )
        bufferedGeometriesForDissolve << outputGeometry;
      else
        out.setGeometry( outputGeometry );
    }

    if ( !dissolve )
      sink->addFeature( out, QgsFeatureSink::FastInsert );

    feedback->setProgress( current * step );
    current++;
  }

  if ( dissolve )
  {
    QgsGeometry finalGeometry = QgsGeometry::unaryUnion( bufferedGeometriesForDissolve );
    QgsFeature f;
    f.setGeometry( finalGeometry );
    f.setAttributes( dissolveAttrs );
    sink->addFeature( f, QgsFeatureSink::FastInsert );
  }

  QVariantMap outputs;
  outputs.insert( QStringLiteral( "OUTPUT" ), dest );
  return outputs;
}


void QgsDissolveAlgorithm::initAlgorithm( const QVariantMap & )
{
  addParameter( new QgsProcessingParameterFeatureSource( QStringLiteral( "INPUT" ), QObject::tr( "Input layer" ) ) );
  addParameter( new QgsProcessingParameterField( QStringLiteral( "FIELD" ), QObject::tr( "Unique ID fields" ), QVariant(),
                QStringLiteral( "INPUT" ), QgsProcessingParameterField::Any, true, true ) );

  addParameter( new QgsProcessingParameterFeatureSink( QStringLiteral( "OUTPUT" ), QObject::tr( "Dissolved" ) ) );
}

QString QgsDissolveAlgorithm::shortHelpString() const
{
  return QObject::tr( "This algorithm takes a polygon or line vector layer and combines their geometries into new geometries. One or more attributes can "
                      "be specified to dissolve only geometries belonging to the same class (having the same value for the specified attributes), alternatively "
                      "all geometries can be dissolved.\n\n"
                      "All output geometries will be converted to multi geometries. "
                      "In case the input is a polygon layer, common boundaries of adjacent polygons being dissolved will get erased." );
}

QgsDissolveAlgorithm *QgsDissolveAlgorithm::createInstance() const
{
  return new QgsDissolveAlgorithm();
}

QVariantMap QgsCollectorAlgorithm::processCollection( const QVariantMap &parameters, QgsProcessingContext &context, QgsProcessingFeedback *feedback,
    std::function<QgsGeometry( const QList< QgsGeometry >& )> collector, int maxQueueLength )
{
  std::unique_ptr< QgsFeatureSource > source( parameterAsSource( parameters, QStringLiteral( "INPUT" ), context ) );
  if ( !source )
    return QVariantMap();

  QString dest;
  std::unique_ptr< QgsFeatureSink > sink( parameterAsSink( parameters, QStringLiteral( "OUTPUT" ), context, dest, source->fields(), QgsWkbTypes::multiType( source->wkbType() ), source->sourceCrs() ) );

  if ( !sink )
    return QVariantMap();

  QStringList fields = parameterAsFields( parameters, QStringLiteral( "FIELD" ), context );

  long count = source->featureCount();

  QgsFeature f;
  QgsFeatureIterator it = source->getFeatures();

  double step = count > 0 ? 100.0 / count : 1;
  int current = 0;

  if ( fields.isEmpty() )
  {
    // dissolve all - not using fields
    bool firstFeature = true;
    // we dissolve geometries in blocks using unaryUnion
    QList< QgsGeometry > geomQueue;
    QgsFeature outputFeature;

    while ( it.nextFeature( f ) )
    {
      if ( feedback->isCanceled() )
      {
        break;
      }

      if ( firstFeature )
      {
        outputFeature = f;
        firstFeature = false;
      }

      if ( f.hasGeometry() && f.geometry() )
      {
        geomQueue.append( f.geometry() );
        if ( maxQueueLength > 0 && geomQueue.length() > maxQueueLength )
        {
          // queue too long, combine it
          QgsGeometry tempOutputGeometry = collector( geomQueue );
          geomQueue.clear();
          geomQueue << tempOutputGeometry;
        }
      }

      feedback->setProgress( current * step );
      current++;
    }

    outputFeature.setGeometry( collector( geomQueue ) );
    sink->addFeature( outputFeature, QgsFeatureSink::FastInsert );
  }
  else
  {
    QList< int > fieldIndexes;
    Q_FOREACH ( const QString &field, fields )
    {
      int index = source->fields().lookupField( field );
      if ( index >= 0 )
        fieldIndexes << index;
    }

    QHash< QVariant, QgsAttributes > attributeHash;
    QHash< QVariant, QList< QgsGeometry > > geometryHash;

    while ( it.nextFeature( f ) )
    {
      if ( feedback->isCanceled() )
      {
        break;
      }

      QVariantList indexAttributes;
      Q_FOREACH ( int index, fieldIndexes )
      {
        indexAttributes << f.attribute( index );
      }

      if ( !attributeHash.contains( indexAttributes ) )
      {
        // keep attributes of first feature
        attributeHash.insert( indexAttributes, f.attributes() );
      }

      if ( f.hasGeometry() && f.geometry() )
      {
        geometryHash[ indexAttributes ].append( f.geometry() );
      }
    }

    int numberFeatures = attributeHash.count();
    QHash< QVariant, QgsAttributes >::const_iterator attrIt = attributeHash.constBegin();
    for ( ; attrIt != attributeHash.constEnd(); ++attrIt )
    {
      if ( feedback->isCanceled() )
      {
        break;
      }

      QgsFeature outputFeature;
      if ( geometryHash.contains( attrIt.key() ) )
      {
        QgsGeometry geom = collector( geometryHash.value( attrIt.key() ) );
        if ( !geom.isMultipart() )
        {
          geom.convertToMultiType();
        }
        outputFeature.setGeometry( geom );
      }
      outputFeature.setAttributes( attrIt.value() );
      sink->addFeature( outputFeature, QgsFeatureSink::FastInsert );

      feedback->setProgress( current * 100.0 / numberFeatures );
      current++;
    }
  }

  QVariantMap outputs;
  outputs.insert( QStringLiteral( "OUTPUT" ), dest );
  return outputs;
}

QVariantMap QgsDissolveAlgorithm::processAlgorithm( const QVariantMap &parameters, QgsProcessingContext &context, QgsProcessingFeedback *feedback )
{
  return processCollection( parameters, context, feedback, []( const QList< QgsGeometry > &parts )->QgsGeometry
  {
    return QgsGeometry::unaryUnion( parts );
  }, 10000 );
}

QVariantMap QgsCollectAlgorithm::processAlgorithm( const QVariantMap &parameters, QgsProcessingContext &context, QgsProcessingFeedback *feedback )
{
  return processCollection( parameters, context, feedback, []( const QList< QgsGeometry > &parts )->QgsGeometry
  {
    return QgsGeometry::collectGeometry( parts );
  } );
}


void QgsClipAlgorithm::initAlgorithm( const QVariantMap & )
{
  addParameter( new QgsProcessingParameterFeatureSource( QStringLiteral( "INPUT" ), QObject::tr( "Input layer" ) ) );
  addParameter( new QgsProcessingParameterFeatureSource( QStringLiteral( "OVERLAY" ), QObject::tr( "Clip layer" ), QList< int >() << QgsProcessing::TypeVectorPolygon ) );

  addParameter( new QgsProcessingParameterFeatureSink( QStringLiteral( "OUTPUT" ), QObject::tr( "Clipped" ) ) );
}

QString QgsClipAlgorithm::shortHelpString() const
{
  return QObject::tr( "This algorithm clips a vector layer using the polygons of an additional polygons layer. Only the parts of the features "
                      "in the input layer that falls within the polygons of the clipping layer will be added to the resulting layer.\n\n"
                      "The attributes of the features are not modified, although properties such as area or length of the features will "
                      "be modified by the clipping operation. If such properties are stored as attributes, those attributes will have to "
                      "be manually updated." );
}

QgsClipAlgorithm *QgsClipAlgorithm::createInstance() const
{
  return new QgsClipAlgorithm();
}

QVariantMap QgsClipAlgorithm::processAlgorithm( const QVariantMap &parameters, QgsProcessingContext &context, QgsProcessingFeedback *feedback )
{
  std::unique_ptr< QgsFeatureSource > featureSource( parameterAsSource( parameters, QStringLiteral( "INPUT" ), context ) );
  if ( !featureSource )
    return QVariantMap();

  std::unique_ptr< QgsFeatureSource > maskSource( parameterAsSource( parameters, QStringLiteral( "OVERLAY" ), context ) );
  if ( !maskSource )
    return QVariantMap();

  QString dest;
  std::unique_ptr< QgsFeatureSink > sink( parameterAsSink( parameters, QStringLiteral( "OUTPUT" ), context, dest, featureSource->fields(), QgsWkbTypes::multiType( featureSource->wkbType() ), featureSource->sourceCrs() ) );

  if ( !sink )
    return QVariantMap();

  // first build up a list of clip geometries
  QList< QgsGeometry > clipGeoms;
  QgsFeatureIterator it = maskSource->getFeatures( QgsFeatureRequest().setSubsetOfAttributes( QList< int >() ).setDestinationCrs( featureSource->sourceCrs() ) );
  QgsFeature f;
  while ( it.nextFeature( f ) )
  {
    if ( f.hasGeometry() )
      clipGeoms << f.geometry();
  }

  QVariantMap outputs;
  outputs.insert( QStringLiteral( "OUTPUT" ), dest );

  if ( clipGeoms.isEmpty() )
    return outputs;

  // are we clipping against a single feature? if so, we can show finer progress reports
  bool singleClipFeature = false;
  QgsGeometry combinedClipGeom;
  if ( clipGeoms.length() > 1 )
  {
    combinedClipGeom = QgsGeometry::unaryUnion( clipGeoms );
    singleClipFeature = false;
  }
  else
  {
    combinedClipGeom = clipGeoms.at( 0 );
    singleClipFeature = true;
  }

  // use prepared geometries for faster intersection tests
  std::unique_ptr< QgsGeometryEngine > engine( QgsGeometry::createGeometryEngine( combinedClipGeom.geometry() ) );
  engine->prepareGeometry();

  QgsFeatureIds testedFeatureIds;

  int i = -1;
  Q_FOREACH ( const QgsGeometry &clipGeom, clipGeoms )
  {
    i++;
    if ( feedback->isCanceled() )
    {
      break;
    }

    QgsFeatureIterator inputIt = featureSource->getFeatures( QgsFeatureRequest().setFilterRect( clipGeom.boundingBox() ) );
    QgsFeatureList inputFeatures;
    QgsFeature f;
    while ( inputIt.nextFeature( f ) )
      inputFeatures << f;

    if ( inputFeatures.isEmpty() )
      continue;

    double step = 0;
    if ( singleClipFeature )
      step = 100.0 / inputFeatures.length();

    int current = 0;
    Q_FOREACH ( const QgsFeature &inputFeature, inputFeatures )
    {
      if ( feedback->isCanceled() )
      {
        break;
      }

      if ( !inputFeature.hasGeometry() )
        continue;

      if ( testedFeatureIds.contains( inputFeature.id() ) )
      {
        // don't retest a feature we have already checked
        continue;
      }
      testedFeatureIds.insert( inputFeature.id() );

      if ( !engine->intersects( inputFeature.geometry().geometry() ) )
        continue;

      QgsGeometry newGeometry;
      if ( !engine->contains( inputFeature.geometry().geometry() ) )
      {
        QgsGeometry currentGeometry = inputFeature.geometry();
        newGeometry = combinedClipGeom.intersection( currentGeometry );
        if ( newGeometry.wkbType() == QgsWkbTypes::Unknown || QgsWkbTypes::flatType( newGeometry.geometry()->wkbType() ) == QgsWkbTypes::GeometryCollection )
        {
          QgsGeometry intCom = inputFeature.geometry().combine( newGeometry );
          QgsGeometry intSym = inputFeature.geometry().symDifference( newGeometry );
          newGeometry = intCom.difference( intSym );
        }
      }
      else
      {
        // clip geometry totally contains feature geometry, so no need to perform intersection
        newGeometry = inputFeature.geometry();
      }

      QgsFeature outputFeature;
      outputFeature.setGeometry( newGeometry );
      outputFeature.setAttributes( inputFeature.attributes() );
      sink->addFeature( outputFeature, QgsFeatureSink::FastInsert );


      if ( singleClipFeature )
        feedback->setProgress( current * step );
    }

    if ( !singleClipFeature )
    {
      // coarse progress report for multiple clip geometries
      feedback->setProgress( 100.0 * static_cast< double >( i ) / clipGeoms.length() );
    }
  }

  return outputs;
}


void QgsTransformAlgorithm::initParameters( const QVariantMap & )
{
  addParameter( new QgsProcessingParameterCrs( QStringLiteral( "TARGET_CRS" ), QObject::tr( "Target CRS" ), QStringLiteral( "EPSG:4326" ) ) );
}

QString QgsTransformAlgorithm::shortHelpString() const
{
  return QObject::tr( "This algorithm reprojects a vector layer. It creates a new layer with the same features "
                      "as the input one, but with geometries reprojected to a new CRS.\n\n"
                      "Attributes are not modified by this algorithm." );
}

QgsTransformAlgorithm *QgsTransformAlgorithm::createInstance() const
{
  return new QgsTransformAlgorithm();
}

bool QgsTransformAlgorithm::prepareAlgorithm( const QVariantMap &parameters, QgsProcessingContext &context, QgsProcessingFeedback * )
{
  mDestCrs = parameterAsCrs( parameters, QStringLiteral( "TARGET_CRS" ), context );
  return true;
}

QgsFeature QgsTransformAlgorithm::processFeature( const QgsFeature &f, QgsProcessingFeedback * )
{
  QgsFeature feature = f;
  if ( !mCreatedTransform )
  {
    mCreatedTransform = true;
    mTransform = QgsCoordinateTransform( sourceCrs(), mDestCrs );
  }

  if ( feature.hasGeometry() )
  {
    QgsGeometry g = feature.geometry();
    if ( g.transform( mTransform ) == 0 )
    {
      feature.setGeometry( g );
    }
    else
    {
      feature.clearGeometry();
    }
  }
  return feature;
}


void QgsSubdivideAlgorithm::initParameters( const QVariantMap & )
{
  addParameter( new QgsProcessingParameterNumber( QStringLiteral( "MAX_NODES" ), QObject::tr( "Maximum nodes in parts" ), QgsProcessingParameterNumber::Integer,
                256, false, 8, 100000 ) );
}

QString QgsSubdivideAlgorithm::shortHelpString() const
{
  return QObject::tr( "Subdivides the geometry. The returned geometry will be a collection containing subdivided parts "
                      "from the original geometry, where no part has more then the specified maximum number of nodes.\n\n"
                      "This is useful for dividing a complex geometry into less complex parts, which are better able to be spatially "
                      "indexed and faster to perform further operations such as intersects on. The returned geometry parts may "
                      "not be valid and may contain self-intersections.\n\n"
                      "Curved geometries will be segmentized before subdivision." );
}

QgsSubdivideAlgorithm *QgsSubdivideAlgorithm::createInstance() const
{
  return new QgsSubdivideAlgorithm();
}

QgsWkbTypes::Type QgsSubdivideAlgorithm::outputWkbType( QgsWkbTypes::Type inputWkbType ) const
{
  return QgsWkbTypes::multiType( inputWkbType );
}

QgsFeature QgsSubdivideAlgorithm::processFeature( const QgsFeature &f, QgsProcessingFeedback *feedback )
{
  QgsFeature feature = f;
  if ( feature.hasGeometry() )
  {
    feature.setGeometry( feature.geometry().subdivide( mMaxNodes ) );
    if ( !feature.geometry() )
    {
      feedback->reportError( QObject::tr( "Error calculating subdivision for feature %1" ).arg( feature.id() ) );
    }
  }
  return feature;
}

bool QgsSubdivideAlgorithm::prepareAlgorithm( const QVariantMap &parameters, QgsProcessingContext &context, QgsProcessingFeedback * )
{
  mMaxNodes = parameterAsInt( parameters, QStringLiteral( "MAX_NODES" ), context );
  return true;
}


void QgsMultipartToSinglepartAlgorithm::initAlgorithm( const QVariantMap & )
{
  addParameter( new QgsProcessingParameterFeatureSource( QStringLiteral( "INPUT" ), QObject::tr( "Input layer" ) ) );

  addParameter( new QgsProcessingParameterFeatureSink( QStringLiteral( "OUTPUT" ), QObject::tr( "Single parts" ) ) );
}

QString QgsMultipartToSinglepartAlgorithm::shortHelpString() const
{
  return QObject::tr( "This algorithm takes a vector layer with multipart geometries and generates a new one in which all geometries contain "
                      "a single part. Features with multipart geometries are divided in as many different features as parts the geometry "
                      "contain, and the same attributes are used for each of them." );
}

QgsMultipartToSinglepartAlgorithm *QgsMultipartToSinglepartAlgorithm::createInstance() const
{
  return new QgsMultipartToSinglepartAlgorithm();
}

QVariantMap QgsMultipartToSinglepartAlgorithm::processAlgorithm( const QVariantMap &parameters, QgsProcessingContext &context, QgsProcessingFeedback *feedback )
{
  std::unique_ptr< QgsFeatureSource > source( parameterAsSource( parameters, QStringLiteral( "INPUT" ), context ) );
  if ( !source )
    return QVariantMap();

  QgsWkbTypes::Type sinkType = QgsWkbTypes::singleType( source->wkbType() );

  QString dest;
  std::unique_ptr< QgsFeatureSink > sink( parameterAsSink( parameters, QStringLiteral( "OUTPUT" ), context, dest, source->fields(),
                                          sinkType, source->sourceCrs() ) );
  if ( !sink )
    return QVariantMap();

  long count = source->featureCount();

  QgsFeature f;
  QgsFeatureIterator it = source->getFeatures();

  double step = count > 0 ? 100.0 / count : 1;
  int current = 0;
  while ( it.nextFeature( f ) )
  {
    if ( feedback->isCanceled() )
    {
      break;
    }

    QgsFeature out = f;
    if ( out.hasGeometry() )
    {
      QgsGeometry inputGeometry = f.geometry();
      if ( inputGeometry.isMultipart() )
      {
        Q_FOREACH ( const QgsGeometry &g, inputGeometry.asGeometryCollection() )
        {
          out.setGeometry( g );
          sink->addFeature( out, QgsFeatureSink::FastInsert );
        }
      }
      else
      {
        sink->addFeature( out, QgsFeatureSink::FastInsert );
      }
    }
    else
    {
      // feature with null geometry
      sink->addFeature( out, QgsFeatureSink::FastInsert );
    }

    feedback->setProgress( current * step );
    current++;
  }

  QVariantMap outputs;
  outputs.insert( QStringLiteral( "OUTPUT" ), dest );
  return outputs;
}


void QgsExtractByExpressionAlgorithm::initAlgorithm( const QVariantMap & )
{
  addParameter( new QgsProcessingParameterFeatureSource( QStringLiteral( "INPUT" ), QObject::tr( "Input layer" ) ) );
  addParameter( new QgsProcessingParameterExpression( QStringLiteral( "EXPRESSION" ), QObject::tr( "Expression" ), QVariant(), QStringLiteral( "INPUT" ) ) );

  addParameter( new QgsProcessingParameterFeatureSink( QStringLiteral( "OUTPUT" ), QObject::tr( "Matching features" ) ) );
  QgsProcessingParameterFeatureSink *failOutput = new QgsProcessingParameterFeatureSink( QStringLiteral( "FAIL_OUTPUT" ),  QObject::tr( "Non-matching" ),
      QgsProcessing::TypeVectorAnyGeometry, QVariant(), true );
  failOutput->setCreateByDefault( false );
  addParameter( failOutput );
}

QString QgsExtractByExpressionAlgorithm::shortHelpString() const
{
  return QObject::tr( "This algorithm creates a new vector layer that only contains matching features from an input layer. "
                      "The criteria for adding features to the resulting layer is based on a QGIS expression.\n\n"
                      "For more information about expressions see the <a href =\"{qgisdocs}/user_manual/working_with_vector/expression.html\">user manual</a>" );
}

QgsExtractByExpressionAlgorithm *QgsExtractByExpressionAlgorithm::createInstance() const
{
  return new QgsExtractByExpressionAlgorithm();
}

QVariantMap QgsExtractByExpressionAlgorithm::processAlgorithm( const QVariantMap &parameters, QgsProcessingContext &context, QgsProcessingFeedback *feedback )
{
  std::unique_ptr< QgsFeatureSource > source( parameterAsSource( parameters, QStringLiteral( "INPUT" ), context ) );
  if ( !source )
    return QVariantMap();

  QString expressionString = parameterAsExpression( parameters, QStringLiteral( "EXPRESSION" ), context );

  QString matchingSinkId;
  std::unique_ptr< QgsFeatureSink > matchingSink( parameterAsSink( parameters, QStringLiteral( "OUTPUT" ), context, matchingSinkId, source->fields(),
      source->wkbType(), source->sourceCrs() ) );
  if ( !matchingSink )
    return QVariantMap();

  QString nonMatchingSinkId;
  std::unique_ptr< QgsFeatureSink > nonMatchingSink( parameterAsSink( parameters, QStringLiteral( "FAIL_OUTPUT" ), context, nonMatchingSinkId, source->fields(),
      source->wkbType(), source->sourceCrs() ) );

  QgsExpression expression( expressionString );
  if ( expression.hasParserError() )
  {
    throw QgsProcessingException( expression.parserErrorString() );
  }

  QgsExpressionContext expressionContext = createExpressionContext( parameters, context );

  long count = source->featureCount();

  double step = count > 0 ? 100.0 / count : 1;
  int current = 0;

  if ( !nonMatchingSink )
  {
    // not saving failing features - so only fetch good features
    QgsFeatureRequest req;
    req.setFilterExpression( expressionString );
    req.setExpressionContext( expressionContext );

    QgsFeatureIterator it = source->getFeatures( req );
    QgsFeature f;
    while ( it.nextFeature( f ) )
    {
      if ( feedback->isCanceled() )
      {
        break;
      }

      matchingSink->addFeature( f, QgsFeatureSink::FastInsert );

      feedback->setProgress( current * step );
      current++;
    }
  }
  else
  {
    // saving non-matching features, so we need EVERYTHING
    expressionContext.setFields( source->fields() );
    expression.prepare( &expressionContext );

    QgsFeatureIterator it = source->getFeatures();
    QgsFeature f;
    while ( it.nextFeature( f ) )
    {
      if ( feedback->isCanceled() )
      {
        break;
      }

      expressionContext.setFeature( f );
      if ( expression.evaluate( &expressionContext ).toBool() )
      {
        matchingSink->addFeature( f, QgsFeatureSink::FastInsert );
      }
      else
      {
        nonMatchingSink->addFeature( f, QgsFeatureSink::FastInsert );
      }

      feedback->setProgress( current * step );
      current++;
    }
  }


  QVariantMap outputs;
  outputs.insert( QStringLiteral( "OUTPUT" ), matchingSinkId );
  if ( nonMatchingSink )
    outputs.insert( QStringLiteral( "FAIL_OUTPUT" ), nonMatchingSinkId );
  return outputs;
}


void QgsExtractByAttributeAlgorithm::initAlgorithm( const QVariantMap & )
{
  addParameter( new QgsProcessingParameterFeatureSource( QStringLiteral( "INPUT" ), QObject::tr( "Input layer" ) ) );
  addParameter( new QgsProcessingParameterField( QStringLiteral( "FIELD" ), QObject::tr( "Selection attribute" ), QVariant(), QStringLiteral( "INPUT" ) ) );
  addParameter( new QgsProcessingParameterEnum( QStringLiteral( "OPERATOR" ), QObject::tr( "Operator" ), QStringList()
                << QObject::tr( "=" )
                << QObject::trUtf8( "≠" )
                << QObject::tr( ">" )
                << QObject::tr( ">=" )
                << QObject::tr( "<" )
                << QObject::tr( "<=" )
                << QObject::tr( "begins with" )
                << QObject::tr( "contains" )
                << QObject::tr( "is null" )
                << QObject::tr( "is not null" )
                << QObject::tr( "does not contain" ) ) );
  addParameter( new QgsProcessingParameterString( QStringLiteral( "VALUE" ), QObject::tr( "Value" ), QVariant(), false, true ) );

  addParameter( new QgsProcessingParameterFeatureSink( QStringLiteral( "OUTPUT" ), QObject::tr( "Extracted (attribute)" ) ) );
  QgsProcessingParameterFeatureSink *failOutput = new QgsProcessingParameterFeatureSink( QStringLiteral( "FAIL_OUTPUT" ),  QObject::tr( "Extracted (non-matching)" ),
      QgsProcessing::TypeVectorAnyGeometry, QVariant(), true );
  failOutput->setCreateByDefault( false );
  addParameter( failOutput );
}

QString QgsExtractByAttributeAlgorithm::shortHelpString() const
{
  return QObject::tr( "  This algorithm creates a new vector layer that only contains matching features from an input layer. "
                      "The criteria for adding features to the resulting layer is defined based on the values "
                      "of an attribute from the input layer." );
}

QgsExtractByAttributeAlgorithm *QgsExtractByAttributeAlgorithm::createInstance() const
{
  return new QgsExtractByAttributeAlgorithm();
}

QVariantMap QgsExtractByAttributeAlgorithm::processAlgorithm( const QVariantMap &parameters, QgsProcessingContext &context, QgsProcessingFeedback *feedback )
{
  std::unique_ptr< QgsFeatureSource > source( parameterAsSource( parameters, QStringLiteral( "INPUT" ), context ) );
  if ( !source )
    return QVariantMap();

  QString fieldName = parameterAsString( parameters, QStringLiteral( "FIELD" ), context );
  Operation op = static_cast< Operation >( parameterAsEnum( parameters, QStringLiteral( "OPERATOR" ), context ) );
  QString value = parameterAsString( parameters, QStringLiteral( "VALUE" ), context );

  QString matchingSinkId;
  std::unique_ptr< QgsFeatureSink > matchingSink( parameterAsSink( parameters, QStringLiteral( "OUTPUT" ), context, matchingSinkId, source->fields(),
      source->wkbType(), source->sourceCrs() ) );
  if ( !matchingSink )
    return QVariantMap();

  QString nonMatchingSinkId;
  std::unique_ptr< QgsFeatureSink > nonMatchingSink( parameterAsSink( parameters, QStringLiteral( "FAIL_OUTPUT" ), context, nonMatchingSinkId, source->fields(),
      source->wkbType(), source->sourceCrs() ) );


  int idx = source->fields().lookupField( fieldName );
  QVariant::Type fieldType = source->fields().at( idx ).type();

  if ( fieldType != QVariant::String && ( op == BeginsWith || op == Contains || op == DoesNotContain ) )
  {
    QString method;
    switch ( op )
    {
      case BeginsWith:
        method = QObject::tr( "begins with" );
        break;
      case Contains:
        method = QObject::tr( "contains" );
        break;
      case DoesNotContain:
        method = QObject::tr( "does not contain" );
        break;

      default:
        break;
    }

    throw QgsProcessingException( QObject::tr( "Operator '%1' can be used only with string fields." ).arg( method ) );
  }

  QString fieldRef = QgsExpression::quotedColumnRef( fieldName );
  QString quotedVal = QgsExpression::quotedValue( value );
  QString expr;
  switch ( op )
  {
    case Equals:
      expr = QStringLiteral( "%1 = %3" ).arg( fieldRef, quotedVal );
      break;
    case  NotEquals:
      expr = QStringLiteral( "%1 != %3" ).arg( fieldRef, quotedVal );
      break;
    case GreaterThan:
      expr = QStringLiteral( "%1 > %3" ).arg( fieldRef, quotedVal );
      break;
    case GreaterThanEqualTo:
      expr = QStringLiteral( "%1 >= %3" ).arg( fieldRef, quotedVal );
      break;
    case LessThan:
      expr = QStringLiteral( "%1 < %3" ).arg( fieldRef, quotedVal );
      break;
    case LessThanEqualTo:
      expr = QStringLiteral( "%1 <= %3" ).arg( fieldRef, quotedVal );
      break;
    case BeginsWith:
      expr = QStringLiteral( "%1 LIKE '%2%'" ).arg( fieldRef, value );
      break;
    case Contains:
      expr = QStringLiteral( "%1 LIKE '%%2%'" ).arg( fieldRef, value );
      break;
    case IsNull:
      expr = QStringLiteral( "%1 IS NULL" ).arg( fieldRef );
      break;
    case IsNotNull:
      expr = QStringLiteral( "%1 IS NOT NULL" ).arg( fieldRef );
      break;
    case DoesNotContain:
      expr = QStringLiteral( "%1 NOT LIKE '%%2%'" ).arg( fieldRef, value );
      break;
  }

  QgsExpression expression( expr );
  if ( expression.hasParserError() )
  {
    throw QgsProcessingException( expression.parserErrorString() );
  }

  QgsExpressionContext expressionContext = createExpressionContext( parameters, context );

  long count = source->featureCount();

  double step = count > 0 ? 100.0 / count : 1;
  int current = 0;

  if ( !nonMatchingSink )
  {
    // not saving failing features - so only fetch good features
    QgsFeatureRequest req;
    req.setFilterExpression( expr );
    req.setExpressionContext( expressionContext );

    QgsFeatureIterator it = source->getFeatures( req );
    QgsFeature f;
    while ( it.nextFeature( f ) )
    {
      if ( feedback->isCanceled() )
      {
        break;
      }

      matchingSink->addFeature( f, QgsFeatureSink::FastInsert );

      feedback->setProgress( current * step );
      current++;
    }
  }
  else
  {
    // saving non-matching features, so we need EVERYTHING
    expressionContext.setFields( source->fields() );
    expression.prepare( &expressionContext );

    QgsFeatureIterator it = source->getFeatures();
    QgsFeature f;
    while ( it.nextFeature( f ) )
    {
      if ( feedback->isCanceled() )
      {
        break;
      }

      expressionContext.setFeature( f );
      if ( expression.evaluate( &expressionContext ).toBool() )
      {
        matchingSink->addFeature( f, QgsFeatureSink::FastInsert );
      }
      else
      {
        nonMatchingSink->addFeature( f, QgsFeatureSink::FastInsert );
      }

      feedback->setProgress( current * step );
      current++;
    }
  }


  QVariantMap outputs;
  outputs.insert( QStringLiteral( "OUTPUT" ), matchingSinkId );
  if ( nonMatchingSink )
    outputs.insert( QStringLiteral( "FAIL_OUTPUT" ), nonMatchingSinkId );
  return outputs;
}


void QgsRemoveNullGeometryAlgorithm::initAlgorithm( const QVariantMap & )
{
  addParameter( new QgsProcessingParameterFeatureSource( QStringLiteral( "INPUT" ), QObject::tr( "Input layer" ) ) );

  addParameter( new QgsProcessingParameterFeatureSink( QStringLiteral( "OUTPUT" ), QObject::tr( "Non null geometries" ),
                QgsProcessing::TypeVectorAnyGeometry, QVariant(), true ) );
  QgsProcessingParameterFeatureSink *nullOutput = new QgsProcessingParameterFeatureSink( QStringLiteral( "NULL_OUTPUT" ),  QObject::tr( "Null geometries" ),
      QgsProcessing::TypeVector, QVariant(), true );
  nullOutput->setCreateByDefault( false );
  addParameter( nullOutput );
}

QString QgsRemoveNullGeometryAlgorithm::shortHelpString() const
{
  return QObject::tr( "This algorithm removes any features which do not have a geometry from a vector layer. "
                      "All other features will be copied unchanged.\n\n"
                      "Optionally, the features with null geometries can be saved to a separate output." );
}

QgsRemoveNullGeometryAlgorithm *QgsRemoveNullGeometryAlgorithm::createInstance() const
{
  return new QgsRemoveNullGeometryAlgorithm();
}

QVariantMap QgsRemoveNullGeometryAlgorithm::processAlgorithm( const QVariantMap &parameters, QgsProcessingContext &context, QgsProcessingFeedback *feedback )
{
  std::unique_ptr< QgsFeatureSource > source( parameterAsSource( parameters, QStringLiteral( "INPUT" ), context ) );
  if ( !source )
    return QVariantMap();

  QString nonNullSinkId;
  std::unique_ptr< QgsFeatureSink > nonNullSink( parameterAsSink( parameters, QStringLiteral( "OUTPUT" ), context, nonNullSinkId, source->fields(),
      source->wkbType(), source->sourceCrs() ) );

  QString nullSinkId;
  std::unique_ptr< QgsFeatureSink > nullSink( parameterAsSink( parameters, QStringLiteral( "NULL_OUTPUT" ), context, nullSinkId, source->fields() ) );

  long count = source->featureCount();

  double step = count > 0 ? 100.0 / count : 1;
  int current = 0;

  QgsFeature f;
  QgsFeatureIterator it = source->getFeatures();
  while ( it.nextFeature( f ) )
  {
    if ( feedback->isCanceled() )
    {
      break;
    }

    if ( f.hasGeometry() && nonNullSink )
    {
      nonNullSink->addFeature( f, QgsFeatureSink::FastInsert );
    }
    else if ( !f.hasGeometry() && nullSink )
    {
      nullSink->addFeature( f, QgsFeatureSink::FastInsert );
    }

    feedback->setProgress( current * step );
    current++;
  }

  QVariantMap outputs;
  if ( nonNullSink )
    outputs.insert( QStringLiteral( "OUTPUT" ), nonNullSinkId );
  if ( nullSink )
    outputs.insert( QStringLiteral( "NULL_OUTPUT" ), nullSinkId );
  return outputs;
}


QString QgsBoundingBoxAlgorithm::shortHelpString() const
{
  return QObject::tr( "This algorithm calculates the bounding box (envelope) for each feature in an input layer.\n\nSee the 'Minimum bounding geometry' algorithm for a bounding box calculation which covers the whole layer or grouped subsets of features." );
}

QgsBoundingBoxAlgorithm *QgsBoundingBoxAlgorithm::createInstance() const
{
  return new QgsBoundingBoxAlgorithm();
}

QgsFields QgsBoundingBoxAlgorithm::outputFields( const QgsFields &inputFields ) const
{
  QgsFields fields = inputFields;
  fields.append( QgsField( QStringLiteral( "width" ), QVariant::Double, QString(), 20, 6 ) );
  fields.append( QgsField( QStringLiteral( "height" ), QVariant::Double, QString(), 20, 6 ) );
  fields.append( QgsField( QStringLiteral( "area" ), QVariant::Double, QString(), 20, 6 ) );
  fields.append( QgsField( QStringLiteral( "perimeter" ), QVariant::Double, QString(), 20, 6 ) );
  return fields;
}

QgsFeature QgsBoundingBoxAlgorithm::processFeature( const QgsFeature &feature, QgsProcessingFeedback * )
{
  QgsFeature f = feature;
  if ( f.hasGeometry() )
  {
    QgsRectangle bounds = f.geometry().boundingBox();
    QgsGeometry outputGeometry = QgsGeometry::fromRect( bounds );
    f.setGeometry( outputGeometry );
    QgsAttributes attrs = f.attributes();
    attrs << bounds.width()
          << bounds.height()
          << bounds.area()
          << bounds.perimeter();
    f.setAttributes( attrs );
  }
  return f;
}

QString QgsOrientedMinimumBoundingBoxAlgorithm::shortHelpString() const
{
  return QObject::tr( "This algorithm calculates the minimum area rotated rectangle which covers each feature in an input layer.\n\nSee the 'Minimum bounding geometry' algorithm for a oriented bounding box calculation which covers the whole layer or grouped subsets of features." );
}

QgsOrientedMinimumBoundingBoxAlgorithm *QgsOrientedMinimumBoundingBoxAlgorithm::createInstance() const
{
  return new QgsOrientedMinimumBoundingBoxAlgorithm();
}

QgsFields QgsOrientedMinimumBoundingBoxAlgorithm::outputFields( const QgsFields &inputFields ) const
{
  QgsFields fields = inputFields;
  fields.append( QgsField( QStringLiteral( "width" ), QVariant::Double, QString(), 20, 6 ) );
  fields.append( QgsField( QStringLiteral( "height" ), QVariant::Double, QString(), 20, 6 ) );
  fields.append( QgsField( QStringLiteral( "angle" ), QVariant::Double, QString(), 20, 6 ) );
  fields.append( QgsField( QStringLiteral( "area" ), QVariant::Double, QString(), 20, 6 ) );
  fields.append( QgsField( QStringLiteral( "perimeter" ), QVariant::Double, QString(), 20, 6 ) );
  return fields;
}

QgsFeature QgsOrientedMinimumBoundingBoxAlgorithm::processFeature( const QgsFeature &feature, QgsProcessingFeedback * )
{
  QgsFeature f = feature;
  if ( f.hasGeometry() )
  {
    double area = 0;
    double angle = 0;
    double width = 0;
    double height = 0;
    QgsGeometry outputGeometry = f.geometry().orientedMinimumBoundingBox( area, angle, width, height );
    f.setGeometry( outputGeometry );
    QgsAttributes attrs = f.attributes();
    attrs << width
          << height
          << angle
          << area
          << 2 * width + 2 * height;
    f.setAttributes( attrs );
  }
  return f;
}


void QgsMinimumEnclosingCircleAlgorithm::initParameters( const QVariantMap & )
{
  addParameter( new QgsProcessingParameterNumber( QStringLiteral( "SEGMENTS" ), QObject::tr( "Number of segments in circles" ), QgsProcessingParameterNumber::Integer,
                72, false, 8, 100000 ) );
}

QString QgsMinimumEnclosingCircleAlgorithm::shortHelpString() const
{
  return QObject::tr( "This algorithm calculates the minimum enclosing circle which covers each feature in an input layer.\n\nSee the 'Minimum bounding geometry' algorithm for a minimal enclosing circle calculation which covers the whole layer or grouped subsets of features." );
}

QgsMinimumEnclosingCircleAlgorithm *QgsMinimumEnclosingCircleAlgorithm::createInstance() const
{
  return new QgsMinimumEnclosingCircleAlgorithm();
}

QgsFields QgsMinimumEnclosingCircleAlgorithm::outputFields( const QgsFields &inputFields ) const
{
  QgsFields fields = inputFields;
  fields.append( QgsField( QStringLiteral( "radius" ), QVariant::Double, QString(), 20, 6 ) );
  fields.append( QgsField( QStringLiteral( "area" ), QVariant::Double, QString(), 20, 6 ) );
  return fields;
}

bool QgsMinimumEnclosingCircleAlgorithm::prepareAlgorithm( const QVariantMap &parameters, QgsProcessingContext &context, QgsProcessingFeedback * )
{
  mSegments = parameterAsInt( parameters, QStringLiteral( "SEGMENTS" ), context );
  return true;
}

QgsFeature QgsMinimumEnclosingCircleAlgorithm::processFeature( const QgsFeature &feature, QgsProcessingFeedback * )
{
  QgsFeature f = feature;
  if ( f.hasGeometry() )
  {
    double radius = 0;
    QgsPointXY center;
    QgsGeometry outputGeometry = f.geometry().minimalEnclosingCircle( center, radius, mSegments );
    f.setGeometry( outputGeometry );
    QgsAttributes attrs = f.attributes();
    attrs << radius
          << M_PI *radius *radius;
    f.setAttributes( attrs );
  }
  return f;
}

QString QgsConvexHullAlgorithm::shortHelpString() const
{
  return QObject::tr( "This algorithm calculates the convex hull for each feature in an input layer.\n\nSee the 'Minimum bounding geometry' algorithm for a convex hull calculation which covers the whole layer or grouped subsets of features." );
}

QgsConvexHullAlgorithm *QgsConvexHullAlgorithm::createInstance() const
{
  return new QgsConvexHullAlgorithm();
}

QgsFields QgsConvexHullAlgorithm::outputFields( const QgsFields &inputFields ) const
{
  QgsFields fields = inputFields;
  fields.append( QgsField( QStringLiteral( "area" ), QVariant::Double, QString(), 20, 6 ) );
  fields.append( QgsField( QStringLiteral( "perimeter" ), QVariant::Double, QString(), 20, 6 ) );
  return fields;
}

QgsFeature QgsConvexHullAlgorithm::processFeature( const QgsFeature &feature, QgsProcessingFeedback *feedback )
{
  QgsFeature f = feature;
  if ( f.hasGeometry() )
  {
    QgsGeometry outputGeometry = f.geometry().convexHull();
    if ( !outputGeometry )
      feedback->reportError( outputGeometry.lastError() );
    f.setGeometry( outputGeometry );
    if ( outputGeometry )
    {
      QgsAttributes attrs = f.attributes();
      attrs << outputGeometry.geometry()->area()
            << outputGeometry.geometry()->perimeter();
      f.setAttributes( attrs );
    }
  }
  return f;
}


QString QgsPromoteToMultipartAlgorithm::shortHelpString() const
{
  return QObject::tr( "This algorithm takes a vector layer with singlepart geometries and generates a new one in which all geometries are "
                      "multipart. Input features which are already multipart features will remain unchanged.\n\n"
                      "This algorithm can be used to force geometries to multipart types in order to be compatibility with data providers "
                      "with strict singlepart/multipart compatibility checks.\n\n"
                      "See the 'Collect geometries' or 'Aggregate' algorithms for alternative options." );
}

QgsPromoteToMultipartAlgorithm *QgsPromoteToMultipartAlgorithm::createInstance() const
{
  return new QgsPromoteToMultipartAlgorithm();
}

QgsWkbTypes::Type QgsPromoteToMultipartAlgorithm::outputWkbType( QgsWkbTypes::Type inputWkbType ) const
{
  return QgsWkbTypes::multiType( inputWkbType );
}

QgsFeature QgsPromoteToMultipartAlgorithm::processFeature( const QgsFeature &feature, QgsProcessingFeedback * )
{
  QgsFeature f = feature;
  if ( f.hasGeometry() && !f.geometry().isMultipart() )
  {
    QgsGeometry g = f.geometry();
    g.convertToMultiType();
    f.setGeometry( g );
  }
  return f;
}


void QgsCollectAlgorithm::initAlgorithm( const QVariantMap & )
{
  addParameter( new QgsProcessingParameterFeatureSource( QStringLiteral( "INPUT" ), QObject::tr( "Input layer" ) ) );
  addParameter( new QgsProcessingParameterField( QStringLiteral( "FIELD" ), QObject::tr( "Unique ID fields" ), QVariant(),
                QStringLiteral( "INPUT" ), QgsProcessingParameterField::Any, true, true ) );

  addParameter( new QgsProcessingParameterFeatureSink( QStringLiteral( "OUTPUT" ), QObject::tr( "Collected" ) ) );
}

QString QgsCollectAlgorithm::shortHelpString() const
{
  return QObject::tr( "This algorithm takes a vector layer and collects its geometries into new multipart geometries. One or more attributes can "
                      "be specified to collect only geometries belonging to the same class (having the same value for the specified attributes), alternatively "
                      "all geometries can be collected.\n\n"
                      "All output geometries will be converted to multi geometries, even those with just a single part. "
                      "This algorithm does not dissolve overlapping geometries - they will be collected together without modifying the shape of each geometry part.\n\n"
                      "See the 'Promote to multipart' or 'Aggregate' algorithms for alternative options." );
}

QgsCollectAlgorithm *QgsCollectAlgorithm::createInstance() const
{
  return new QgsCollectAlgorithm();
}



void QgsSelectByLocationAlgorithm::initAlgorithm( const QVariantMap & )
{
  QStringList methods = QStringList() << QObject::tr( "creating new selection" )
                        << QObject::tr( "adding to current selection" )
                        << QObject::tr( "select within current selection" )
                        << QObject::tr( "removing from current selection" );

  addParameter( new QgsProcessingParameterVectorLayer( QStringLiteral( "INPUT" ), QObject::tr( "Select features from" ),
                QList< int >() << QgsProcessing::TypeVectorAnyGeometry ) );


  addParameter( new QgsProcessingParameterEnum( QStringLiteral( "PREDICATE" ),
                QObject::tr( "Where the features are (geometric predicate)" ),
                predicateOptionsList(), true, QVariant::fromValue( QList< int >() << 0 ) ) );

  addParameter( new QgsProcessingParameterFeatureSource( QStringLiteral( "INTERSECT" ),
                QObject::tr( "By comparing to the features from" ),
                QList< int >() << QgsProcessing::TypeVectorAnyGeometry ) );

  addParameter( new QgsProcessingParameterEnum( QStringLiteral( "METHOD" ),
                QObject::tr( "Modify current selection by" ),
                methods, false, 0 ) );
}

QString QgsSelectByLocationAlgorithm::shortHelpString() const
{
  return QObject::tr( "This algorithm creates a selection in a vector layer. The criteria for selecting "
                      "features is based on the spatial relationship between each feature and the features in an additional layer." );
}

QgsSelectByLocationAlgorithm *QgsSelectByLocationAlgorithm::createInstance() const
{
  return new QgsSelectByLocationAlgorithm();
}

QVariantMap QgsSelectByLocationAlgorithm::processAlgorithm( const QVariantMap &parameters, QgsProcessingContext &context, QgsProcessingFeedback *feedback )
{
  QgsVectorLayer *selectLayer = parameterAsVectorLayer( parameters, QStringLiteral( "INPUT" ), context );
  QgsVectorLayer::SelectBehavior method = static_cast< QgsVectorLayer::SelectBehavior >( parameterAsEnum( parameters, QStringLiteral( "METHOD" ), context ) );
  QgsFeatureSource *intersectSource = parameterAsSource( parameters, QStringLiteral( "INTERSECT" ), context );
  const QList< int > selectedPredicates = parameterAsEnums( parameters, QStringLiteral( "PREDICATE" ), context );

  QgsFeatureIds selectedIds;
  auto addToSelection = [&]( const QgsFeature & feature )
  {
    selectedIds.insert( feature.id() );
  };
  process( selectLayer, intersectSource, selectedPredicates, addToSelection, true, feedback );

  selectLayer->selectByIds( selectedIds, method );
  QVariantMap results;
  results.insert( QStringLiteral( "OUTPUT" ), parameters.value( QStringLiteral( "INPUT" ) ) );
  return results;
}

void QgsLocationBasedAlgorithm::process( QgsFeatureSource *targetSource,
    QgsFeatureSource *intersectSource,
    const QList< int > &selectedPredicates,
    std::function < void( const QgsFeature & ) > handleFeatureFunction,
    bool onlyRequireTargetIds,
    QgsFeedback *feedback )
{
  // build a list of 'reversed' predicates, because in this function
  // we actually test the reverse of what the user wants (allowing us
  // to prepare geometries and optimise the algorithm)
  QList< Predicate > predicates;
  for ( int i : selectedPredicates )
  {
    predicates << reversePredicate( static_cast< Predicate >( i ) );
  }

  QgsFeatureIds disjointSet;
  if ( predicates.contains( Disjoint ) )
    disjointSet = targetSource->allFeatureIds();

  QgsFeatureIds foundSet;
  QgsFeatureRequest request = QgsFeatureRequest().setSubsetOfAttributes( QgsAttributeList() ).setDestinationCrs( targetSource->sourceCrs() );
  QgsFeatureIterator fIt = intersectSource->getFeatures( request );
  double step = intersectSource->featureCount() > 0 ? 100.0 / intersectSource->featureCount() : 1;
  int current = 0;
  QgsFeature f;
  std::unique_ptr< QgsGeometryEngine > engine;
  while ( fIt.nextFeature( f ) )
  {
    if ( feedback->isCanceled() )
      break;

    if ( !f.hasGeometry() )
      continue;

    engine.reset();

    QgsRectangle bbox = f.geometry().boundingBox();
    request = QgsFeatureRequest().setFilterRect( bbox );
    if ( onlyRequireTargetIds )
      request.setFlags( QgsFeatureRequest::NoGeometry ).setSubsetOfAttributes( QgsAttributeList() );

    QgsFeatureIterator testFeatureIt = targetSource->getFeatures( request );
    QgsFeature testFeature;
    while ( testFeatureIt.nextFeature( testFeature ) )
    {
      if ( feedback->isCanceled() )
        break;

      if ( foundSet.contains( testFeature.id() ) )
      {
        // already added this one, no need for further tests
        continue;
      }
      if ( predicates.count() == 1 && predicates.at( 0 ) == Disjoint && !disjointSet.contains( testFeature.id() ) )
      {
        // calculating only the disjoint set, and we've already eliminated this feature so no need for further tests
        continue;
      }

      if ( !engine )
      {
        engine.reset( QgsGeometry::createGeometryEngine( f.geometry().geometry() ) );
        engine->prepareGeometry();
      }

      for ( Predicate predicate : qgsAsConst( predicates ) )
      {
        bool isMatch = false;
        switch ( predicate )
        {
          case Intersects:
            isMatch = engine->intersects( testFeature.geometry().geometry() );
            break;
          case Contains:
            isMatch = engine->contains( testFeature.geometry().geometry() );
            break;
          case Disjoint:
            if ( engine->intersects( testFeature.geometry().geometry() ) )
            {
              disjointSet.remove( testFeature.id() );
            }
            break;
          case IsEqual:
            isMatch = engine->isEqual( testFeature.geometry().geometry() );
            break;
          case Touches:
            isMatch = engine->touches( testFeature.geometry().geometry() );
            break;
          case Overlaps:
            isMatch = engine->overlaps( testFeature.geometry().geometry() );
            break;
          case Within:
            isMatch = engine->within( testFeature.geometry().geometry() );
            break;
          case Crosses:
            isMatch = engine->crosses( testFeature.geometry().geometry() );
            break;
        }
        if ( isMatch )
        {
          foundSet.insert( testFeature.id() );
          handleFeatureFunction( testFeature );
        }
      }

    }

    current += 1;
    feedback->setProgress( current * step );
  }

  if ( predicates.contains( Disjoint ) )
  {
    disjointSet = disjointSet.subtract( foundSet );
    QgsFeatureRequest disjointReq = QgsFeatureRequest().setFilterFids( disjointSet );
    if ( onlyRequireTargetIds )
      disjointReq.setSubsetOfAttributes( QgsAttributeList() ).setFlags( QgsFeatureRequest::NoGeometry );
    QgsFeatureIterator disjointIt = targetSource->getFeatures( disjointReq );
    QgsFeature f;
    while ( disjointIt.nextFeature( f ) )
    {
      handleFeatureFunction( f );
    }
  }
}

QgsLocationBasedAlgorithm::Predicate QgsLocationBasedAlgorithm::reversePredicate( QgsLocationBasedAlgorithm::Predicate predicate ) const
{
  switch ( predicate )
  {
    case Intersects:
      return Intersects;
    case Contains:
      return Within;
    case Disjoint:
      return Disjoint;
    case IsEqual:
      return IsEqual;
    case Touches:
      return Touches;
    case Overlaps:
      return Overlaps;
    case Within:
      return Contains;
    case Crosses:
      return Crosses;
  }
  // no warnings
  return Intersects;
}

QStringList QgsLocationBasedAlgorithm::predicateOptionsList() const
{
  return QStringList() << QObject::tr( "intersects" )
         << QObject::tr( "contains" )
         << QObject::tr( "is disjoint" )
         << QObject::tr( "equals" )
         << QObject::tr( "touches" )
         << QObject::tr( "overlaps" )
         << QObject::tr( "within" )
         << QObject::tr( "crosses" );
}



void QgsExtractByLocationAlgorithm::initAlgorithm( const QVariantMap & )
{
  addParameter( new QgsProcessingParameterVectorLayer( QStringLiteral( "INPUT" ), QObject::tr( "Extract features from" ),
                QList< int >() << QgsProcessing::TypeVectorAnyGeometry ) );

  addParameter( new QgsProcessingParameterEnum( QStringLiteral( "PREDICATE" ),
                QObject::tr( "Where the features are (geometric predicate)" ),
                predicateOptionsList(), true, QVariant::fromValue( QList< int >() << 0 ) ) );

  addParameter( new QgsProcessingParameterFeatureSource( QStringLiteral( "INTERSECT" ),
                QObject::tr( "By comparing to the features from" ),
                QList< int >() << QgsProcessing::TypeVectorAnyGeometry ) );

  addParameter( new QgsProcessingParameterFeatureSink( QStringLiteral( "OUTPUT" ), QObject::tr( "Extracted (location)" ) ) );
}

QString QgsExtractByLocationAlgorithm::shortHelpString() const
{
  return QObject::tr( "This algorithm creates a new vector layer that only contains matching features from an "
                      "input layer. The criteria for adding features to the resulting layer is defined "
                      "based on the spatial relationship between each feature and the features in an additional layer." );
}

QgsExtractByLocationAlgorithm *QgsExtractByLocationAlgorithm::createInstance() const
{
  return new QgsExtractByLocationAlgorithm();
}

QVariantMap QgsExtractByLocationAlgorithm::processAlgorithm( const QVariantMap &parameters, QgsProcessingContext &context, QgsProcessingFeedback *feedback )
{
  QgsFeatureSource *input = parameterAsSource( parameters, QStringLiteral( "INPUT" ), context );
  QgsFeatureSource *intersectSource = parameterAsSource( parameters, QStringLiteral( "INTERSECT" ), context );
  const QList< int > selectedPredicates = parameterAsEnums( parameters, QStringLiteral( "PREDICATE" ), context );
  QString dest;
  std::unique_ptr< QgsFeatureSink > sink( parameterAsSink( parameters, QStringLiteral( "OUTPUT" ), context, dest, input->fields(), input->wkbType(), input->sourceCrs() ) );

  if ( !sink )
    return QVariantMap();

  auto addToSink = [&]( const QgsFeature & feature )
  {
    QgsFeature f = feature;
    sink->addFeature( f, QgsFeatureSink::FastInsert );
  };
  process( input, intersectSource, selectedPredicates, addToSink, false, feedback );

  QVariantMap results;
  results.insert( QStringLiteral( "OUTPUT" ), dest );
  return results;
}


///@endcond


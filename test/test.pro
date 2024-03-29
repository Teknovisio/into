# Add tests in alphabetical order

TEMPLATE = subdirs

SUBDIRS = algorithm \
          bits \
          boosting \
          camera \
          calibration \
          classification \
          color \
          colors \
          databasewriter \
          defaultoperation \
          dsp \
          featurecombiner \
          fifo \
          filesystemscanner \
          functional \
          functionoperation \
          fraction \
          genericfunction \
          geometry \
          heap \
          houghtransformoperation \
          httpserver \
          image \
          iterators \
          kdtree \
          kerneladatron \
          kernelperceptron \
          lbp \
          lbpoperation \
          matching \
          math \
          matrix \
          matrixcomposer \
          matrixdecompositions \
          matrixutil \
          multipartdecoder \
          operationcompound \
          optimization \
          perceptron \
          pisooperation \
          planerotation \
          probeinput \
          qimage \
          quantizer \
          ransac \
          readwritelock \
          remoteobject \
          resourcedatabase \
          serialization \
          simplememorymanager \
          socket \
          stereotriangulator \
          stringformatter \
          timer \
          threadsafetimer \
          tracking \
          transforms \
          typetraits \
          universalslot \
          util \
          valueset \
          variant \
          versionnumber \
          video \
          ydin

include(../qt5.pri)
qt5: SUBDIRS += qml

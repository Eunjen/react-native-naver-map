#import "NMFGeometry.h"

#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>

#import "NMFFoundation.h"
#import "NMFTypes.h"

#define NMF_MIN_ZOOM  ((double) 0.0)
#define NMF_MAX_ZOOM  ((double) 21.0)

NS_ASSUME_NONNULL_BEGIN

@class NMFLocationOverlay;
@class NMFCameraPosition;
@class NMFCameraUpdate;
@class NMFProjection;

@protocol NMFMapViewDelegate;
@protocol NMFPickable;

/**
 건물 레이어 그룹. 기본적으로 활성화됩니다.
 @see `NMFMapView.setLayerGroup(:isEnabled:)`
 */
extern NSString *const NMF_LAYER_GROUP_BUILDING;

/**
 대중교통 레이어 그룹.
 @see `NMFMapView.setLayerGroup(:isEnabled:)`
 */
extern NSString *const NMF_LAYER_GROUP_TRANSIT;

/**
 자전거도로 레이어 그룹.
 @see `NMFMapView.setLayerGroup(:isEnabled:)`
 */
extern NSString *const NMF_LAYER_GROUP_BICYCLE;

/**
 실시간 교통정보 레이어 그룹.
 @see `NMFMapView.setLayerGroup(:isEnabled:)`
 */
extern NSString *const NMF_LAYER_GROUP_TRAFFIC;

/**
 지적편집도 레이어 그룹.
 @see `NMFMapView.setLayerGroup(:isEnabled:)`
 */
extern NSString *const NMF_LAYER_GROUP_CADASTRAL;

/**
 등산로 레이어 그룹.
 @see `NMFMapView.setLayerGroup(:isEnabled:)`
 */
extern NSString *const NMF_LAYER_GROUP_MOUNTAIN;

/**
 지도 유형을 나타내는 열거형.
 @see `NMFMapView.mapType`
 */
typedef NS_ENUM(NSInteger, NMFMapType) {
    /** 일반 지도. */
    NMFMapTypeBasic = 0,
    /** 내비게이션 지도. */
    NMFMapTypeNavi,
    /** 위성 지도. */
    NMFMapTypeSatellite,
    /** 위성 지도(겹쳐보기). */
    NMFMapTypeHybrid,
    /** 지형도. */
    NMFMapTypeTerrain
};

/**
 지도 뷰 클래스.
 */
NMF_EXPORT IB_DESIGNABLE
@interface NMFMapView : UIView

/**
 지도 인증 요청. 네트워크 오류 등의 이유로 인증을 재시도할 때 호출합니다.
 @see `NMFAuthManager`
 */
- (void)authorize;

#pragma mark Creating Instances

/**
 프레임 크기로 지도 뷰의 인스턴스를 생성합니다.
 
 @param frame 뷰의 프레임.
 @return `NMFMapView` 인스턴스.
 */
- (instancetype)initWithFrame:(CGRect)frame;

#pragma mark Accessing the Delegate

/**
 `NMFMapView`의 위임자.
 
 지도 뷰의 위임자를 이용해 지도에 오버레이와 관련된 정보나 화면 갱신 여부 등의 메시지를 전달합니다.
 */
@property(nonatomic, weak, nullable) IBOutlet id<NMFMapViewDelegate> delegate;

#pragma mark Configuring the Map’s Appearance

/**
 라이트 모드를 활성화할지 여부를 지정합니다. 라이트 모드가 활성화되면 지도의 로딩이 빨라지고 메모리 소모가
 감소합니다. 그러나 다음과 같은 제약이 생깁니다.
 <ul>
 <li>지도의 전반적인 화질이 하락합니다.</li>
 <li>카메라가 회전하거나 기울어지면 지도 심벌도 함께 회전하거나 기울어집니다.</li>
 <li>줌 레벨이 커지거나 작아지면 지도 심벌도 일정 정도 함께 커지거나 작아집니다.</li>
 <li>`mapType` 지도 유형을 사용할 수 없습니다.</li>
 <li>`setLayerGroup:isEnabled:`, `getLayerGroupEnabled:`, `indoorMapEnabled`, `nightModeEnabled`, `lightness`, `buildingHeight`, `symbolScale`, `symbolPerspectiveRatio`가 동작하지 않습니다.</li>
 <li>`NMFMapViewDelegate.mapView:didTapSymbol:`이 호출되지 않습니다.</li>
 <li>`NMFMarker.isHideCollidedSymbols`가 동작하지 않습니다.</li>
 </ul>
 기본값은 `NO`입니다.
 */
@property(nonatomic) BOOL liteModeEnabled;

/**
 지도의 배경색. 배경은 해당 지역의 지도 데이터가 없거나 로딩 중일 때 나타납니다.
 */
@property (nonatomic) UIColor *backgroundColor;

/**
 지도의 배경 이미지. 배경은 해당 지역의 지도 데이터가 없거나 로딩 중일 때 나타납니다.
 */
@property (nonatomic) UIImage *backgroundImage;

/**
 지도 콘텐츠 영역에 대한 `NMGLatLngBounds`.
 */
@property (nonatomic, readonly, copy) NMGLatLngBounds *contentBounds;

/**
 지도의 패딩. 패딩에 해당하는 부분은 지도의 콘텐츠 영역에서 제외됩니다. 따라서 패딩을 변경하면 비록 화면에 나타나는 지도의 모습은 변하지 않지만
 카메라의 위치는 변경되며, `mapView(:regionWillChangeAnimated:byReason:)`이 호출됩니다.
*/
@property (nonatomic, assign) UIEdgeInsets contentInset;

/**
 지도 뷰의 화면상 너비. pt 단위.
 */
@property (nonatomic, readonly) double mapWidth;

/**
 지도 뷰의 화면상 높이. pt 단위.
 */
@property (nonatomic, readonly) double mapHeight;

/**
 지도 유형.
 
 기본값은 `NMFMapTypeBasic`입니다.
 */
@property(nonatomic) NMFMapType mapType;

/**
 레이어 그룹을 활성화할지 여부를 지정합니다.
 
 @param group 레이어 그룹의 이름.
 @param enabled 활성화할 경우 `YES`, 그렇지 않을 경우 `NO`입니다.
 */
- (void)setLayerGroup:(NSString *)group isEnabled:(BOOL)enabled;

/**
 레이어 그룹이 활성화되어 있는지 여부를 반환합니다.
 
 @param group 레이어 그룹의 이름.
 @return 활성화된 경우 `YES`, 그렇지 않을 경우 `NO`
 */
- (BOOL)getLayerGroupEnabled:(NSString *)group;

/**
 배경의 명도 계수. 값의 범위는 `-1`~`1`.
 계수가 `-1`일 경우 명도 최소치인 검정색으로, `1`일 경우 명도 최대치인 흰색으로 표시됩니다. 오버레이에는 적용되지 않습니다.
 
 기본값 `0`입니다.
 */
@property(nonatomic) CGFloat lightness;

/**
 건물의 3D 높이 배율. 값의 범위는 `0`~`1`. 배율이 `0`일 경우 지도를 기울이더라도 건물이 2D로 나타납니다.
 
 기본값은 `1`입니다.
 */
@property(nonatomic) float buildingHeight;

/**
 야간 모드를 활성화할지 여부. 야간 모드가 활성화되면 지도 스타일이 어둡게 바뀝니다. 지도 유형이 야간
 모드를 지원하지 않으면 활성화하더라도 아무 변화가 일어나지 않습니다.
 
 기본값은 `NO`입니다.
 */
@property(nonatomic, getter=isNightModeEnabled) BOOL nightModeEnabled;

/**
 이 지도의 `NMFLocationOverlay` 객체. 항상 같은 객체를 가리킵니다.
 */
@property (nonatomic, strong, readonly) NMFLocationOverlay *locationOverlay;

/**
 지도의 로캘.
 
 기본값은 시스템 로캘을 의미하는 `nil`입니다.
 */
@property (nonatomic, readwrite, nullable) NSString *locale;


/**
 네이버 로고의 위치를 지정하는 열거형.
 @see `NMFMapView.logoAlign`
 */
typedef NS_ENUM(NSInteger, NMFLogoAlign) {
    /** 지도의 좌하단. */
    NMFLogoAlignLeftBottom = 0,
    /** 지도의 우하단. */
    NMFLogoAlignRightBottom,
    /** 지도의 좌상단. */
    NMFLogoAlignLeftTop,
    /** 지도의 우상단. */
    NMFLogoAlignRightTop
};

/**
 네이버 로고의 위치를 지정합니다.
 
 기본값은 NMFLogoAlignLeftBottom.
 */
@property (nonatomic) NMFLogoAlign logoAlign;

/**
 네이버 로고의 마진을 지정합니다.
 */
@property (nonatomic) UIEdgeInsets logoMargin;

/**
 네이버 로고 클릭을 활성화할지 여부를 지정합니다. 활성화하면 네이버 로고 클릭시 범례, 법적 공지, 오픈소스
 라이선스를 보여주는 알림창이 열립니다.
 
 <strong>이 옵션을 `NO`로 지정하는 앱은 반드시 앱 내에 네이버 지도 SDK의 법적 공지
 (`-showLegalNotice`) 및 오픈소스 라이선스(`-showOpenSourceLicense`)뷰 컨트롤러를 호출하는
 메뉴를 만들어야 합니다.</strong>
 
 기본값은 `YES`입니다.
 */
@property (nonatomic) BOOL logoInteractionEnabled;

/**
 법적 공지를 보여주는 뷰컨트롤러를 호출합니다.
 */
- (void)showLegalNotice;

/**
 오픈소스 라이선스를 보여주는 뷰컨트롤러를 호출합니다.
 */
- (void)showOpenSourceLicense;


/**
 지도가 렌더링되는 속도(fps, frames per second)를 설정합니다.
 
 기본값은 `60`입니다.
 @see `CADisplayLink.preferredFramesPerSecond`
 */
@property (nonatomic, assign) double preferredFramesPerSecond;


#pragma mark Configuring How the User Interacts with the Map

/**
 카메라 이동 애니메이션의 기본 지속 시간. 초 단위.
 
 기본값은 `0.2`입니다.
 */
@property(nonatomic) NSTimeInterval animationDuration;

/**
 줌 제스처가 활성화되어 있는지 여부를 나타내는 속성.
 활성화하면 지도를 더블 탭, 두 손가락 탭, 핀치해 카메라의 줌 레벨을 변경할 수 있습니다.
 
 기본값은 `YES`입니다.
 */
@property(nonatomic, getter=isZoomGestureEnabled) BOOL zoomGestureEnabled;

/**
 스크롤 제스처가 활성화되어 있는지 여부를 나타내는 속성.
 활성화하면 지도를 스와이프해 카메라의 좌표를 변경할 수 있습니다.
 
 기본값은 `YES`입니다.
 */
@property(nonatomic, getter=isScrollGestureEnabled) BOOL scrollGestureEnabled;

/**
 로테이트 제스처가 활성화되어 있는지 여부를 나타내는 속성.
 활성화되면 두 손가락으로 지도를 돌려 카메라의 베어링 각도를 변경할 수 있습니다.
 
 기본값은 `YES`입니다.
 */
@property(nonatomic, getter=isRotateGestureEnabled) BOOL rotateGestureEnabled;

/**
 틸트 제스처가 활성화되어 있는지 여부를 나타내는 속성.
 활성화되면 지도를 두 손가락으로 세로 스와이프해 카메라의 틸트 각도를 변경할 수 있습니다.
 
 기본값은 `YES`입니다.
 */
@property(nonatomic, getter=isTiltGestureEnabled) BOOL tiltGestureEnabled;

/**
 스톱 제스처가 활성화되어 있는지 여부를 나타내는 속성.
 활성화되면 지도 애니메이션 진행 중 탭으로 지도 애니메이션을 중지할 수 있습니다.
 
 기본값은 `YES`입니다.
 */
@property(nonatomic, getter=isStopGestureEnabled) BOOL stopGestureEnabled;

/**
 스크롤 제스처 마찰 계수. 값의 범위는 0~1이며, 계수가 클수록 마찰이 강해집니다.
 
 기본값은 `0.088`입니다.
 */
@property(nonatomic) CGFloat scrollFriction;

/**
 줌 제스처 마찰 계수. 값의 범위는 0~1이며, 계수가 클수록 마찰이 강해집니다.
 
 기본값은 `0.12375`입니다.
 */
@property(nonatomic) CGFloat zoomFriction;

/**
 회전 제스처 마찰 계수. 값의 범위는 0~1이며, 계수가 클수록 마찰이 강해집니다.
 
 기본값은 `0.19333`입니다.
 */
@property(nonatomic) CGFloat rotateFriction;

/**
 지도 클릭 시 피킹되는 `NMFPickable`의 클릭 허용 반경. pt 단위. 사용자가 지도를 클릭했을 때, 클릭된 지점이 `NMFPickable`의
 영역 내에 존재하지 않더라도 허용 반경 내에 있다면 해당 요소가 클릭된 것으로 간주됩니다.
 
 기본값은 `2`입니다.
 */
@property(nonatomic) NSInteger pickTolerance;


#pragma mark Indoor


/**
 실내지도 활성화 여부. 활성화하면 카메라가 일정 크기 이상 확대되고 실내지도가 있는 영역에 포커스될 경우 자동으로 해당 영역의 실내지도가 나타납니다.
 
 기본값은 `NO`입니다.
 */
@property(nonatomic, getter=isIndoorMapEnabled) BOOL indoorMapEnabled;

/**
 실내지도 영역의 포커스 유지 반경. pt 단위. 지정할 경우 카메라의 위치가 포커스 유지 반경을 완전히 벗어날 때까지 영역에 대한 포커스가 유지됩니다.

 기본값은 `20`입니다.
 */
@property(nonatomic) double indoorFocusRadius;



#pragma mark Manipulating the Viewpoint

/**
 지도의 최소 줌 레벨.
 
 기본값은 `0`입니다.
 */
@property (nonatomic) double minZoomLevel;

/**
 지도의 최대 줌 레벨.
 
 기본값은 `20`입니다.
 */
@property (nonatomic) double maxZoomLevel;

/**
 지도의 제한 영역.
 
 기본값은 제한이 없음을 의미하는 `nil`입니다.
 */
@property (nonatomic, nullable) NMGLatLngBounds *extent;

/**
 지도의 콘텐츠 영역에 대한 폴리곤 객체.
 */
@property (nonatomic, copy, readonly) NMGPolygon *contentRegion;

/**
 현재 화면을 커버하는 타일 ID의 목록을 반환합니다.
 
 @return 타일 ID의 목록
 @see NMFTileId
 */
- (NSArray<NSNumber *> *)getCoveringTileIds;

/**
 현재 화면을 커버하는 `zoom` 레벨 타일 ID의 목록을 반환합니다.
 
 @param zoom 줌 레벨.
 @return 타일 ID의 목록
 @see NMFTileId
 */
- (NSArray<NSNumber *> *)getCoveringTileIdsAtZoom:(NSInteger)zoom;

/**
 지도의 콘텐츠 영역 중심에 대한 카메라 위치.
 */
@property (nonatomic, copy, readonly) NMFCameraPosition *cameraPosition;

/**
 이 지도에 대한 `NMFProjection` 객체. 항상 같은 객체가 반환됩니다.
 */
@property (nonatomic, readonly) NMFProjection *projection;

/**
 카메라를 이동합니다. 만약 지도 이동 애니메이션이 진행 중이었다면 취소되고, 진행 중인 `NMFCameraUpdate`의 취소 콜백 블록이 호출됩니다.
 
 @param cameraUpdate `NMFCameraUpdate` 객체.
 */
- (void)moveCamera:(NMFCameraUpdate *)cameraUpdate;

/**
 카메라를 이동합니다. 만약 지도 이동 애니메이션이 진행 중이었다면 취소되고, 진행 중인 `NMFCameraUpdate`의 취소 콜백 블록이 호출됩니다.
 카메라 이동이 완료되거나 취소되었을 경우 `completion` 블록이 호출됩니다.
 
 @param cameraUpdate `NMFCameraUpdate` 객체.
 @param completion   카메라 이동이 완료되었을 때 호출되는 콜백 블록. 애니메이션이 있으면 완전히 끝난 후에 호출됩니다.
                    `isCancelled`는 카메라 이동이 완료되기 전에 다른 카메라 이동이 호출되거나 사용자가 제스처로 지도를 조작한 경우 `YES`입니다.
 */
- (void)moveCamera:(NMFCameraUpdate *)cameraUpdate completion:(nullable void (^)(bool isCancelled))completion;

/**
 현재 진행 중인 지도 이동 애니메이션을 취소합니다.
 */
- (void)cancelTransitions;

#pragma mark Symbol

/**
 지도를 기울일 때 적용되는 심벌의 원근 계수. 계수가 `1`일 경우 배경 지도와 동일한 비율로 멀리 있는
 심벌은 작아지고 가까이 있는 심벌은 커지며, `0`에 가까울수록 원근 효과가 감소합니다.

 기본값은 `1`입니다.
 */
@property (nonatomic) CGFloat symbolPerspectiveRatio;

/**
 심벌의 크기 배율. 배율이 `0.5`일 경우 절반, `2`일 경우 두 배의 크기로 표시됩니다.

 기본값은 `1`입니다.
 */
@property (nonatomic) CGFloat symbolScale;


#pragma mark Pickable Query

/**
 특정 화면 좌표 주변 `tolerance` 내에 나타난 모든 오버레이 및 심벌을 가져옵니다. 목록은 가장 위에
 그려진 요소부터 가장 아래에 그려진 요소의 순으로 정렬됩니다.
 
 @param point 화면 좌표.
 @param tolerance point를 기준으로 한 반경. pt 단위.
 @return 오버레이 및 심벌의 목록. point 주변 tolerance pt 내에 존재하는 오버레이 또는 심벌이 없을 경우 빈 목록.
 */
- (NSArray<id<NMFPickable>> *)pickAll:(CGPoint)point withTolerance:(NSInteger)tolerance;

/**
 특정 화면 좌표에 나타난 모든 오버레이 및 심벌을 가져옵니다. 목록은 가장 위에 그려진 요소부터 가장 아래에 그려진 요소의 순으로 정렬됩니다.
 `pickAll(point, 0)`과 동일합니다.
 
 @param point 화면 좌표.
 @return 오버레이 또는 심벌. point에 존재하는 오버레이 또는 심벌이 없을 경우 nil.
 */
- (nullable id<NMFPickable>)pick:(CGPoint)point;


@end

NS_ASSUME_NONNULL_END


#ifndef YELLOW_DOCUMENT_LAYOUT_HIT_TESTER_H
#define YELLOW_DOCUMENT_LAYOUT_HIT_TESTER_H

#include <SupportDefs.h>
#include <Point.h>

namespace YellowDocumentCore {
    class DocumentNode;
    class DocumentPosition;
}

namespace YellowDocumentLayout {

class HitTester {
public:
    HitTester();
    ~HitTester();

    // Hit test operations
    YellowDocumentCore::DocumentNode* HitTestNode(const BPoint& point);
    YellowDocumentCore::DocumentPosition HitTestPosition(const BPoint& point);
    
    // Find nearest position
    YellowDocumentCore::DocumentPosition FindNearestPosition(const BPoint& point);
};

} // namespace YellowDocumentLayout

#endif // YELLOW_DOCUMENT_LAYOUT_HIT_TESTER_H

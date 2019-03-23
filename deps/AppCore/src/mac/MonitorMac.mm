#import "MonitorMac.h"

namespace ultralight {
    
MonitorMac::MonitorMac() {
    screen_ = [NSScreen mainScreen];
}

double MonitorMac::scale() const {
    return [screen_ backingScaleFactor];
}
    
uint32_t MonitorMac::width() const {
    return (uint32_t)screen_.frame.size.width;
}

uint32_t MonitorMac::height() const {
    return (uint32_t)screen_.frame.size.height;
}
    
}  // namespace ultralight

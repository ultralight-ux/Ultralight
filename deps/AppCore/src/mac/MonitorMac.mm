#import "MonitorMac.h"

namespace ultralight {
    
MonitorMac::MonitorMac() {
    screen_ = [NSScreen mainScreen];
}

double MonitorMac::scale() const {
    return [screen_ backingScaleFactor];
}
    
int MonitorMac::width() const {
    return (int)screen_.frame.size.width;
}

int MonitorMac::height() const {
    return (int)screen_.frame.size.height;
}
    
}  // namespace ultralight

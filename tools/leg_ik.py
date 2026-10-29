"""Python copy of the leg IK, used to cross-check the C version and for plots."""
import math

THIGH_MM = 55.0
SHANK_MM = 55.0


def ik_leg(x, z):
    """Return (hip, knee, ankle) in degrees or None if out of reach."""
    l1, l2 = THIGH_MM, SHANK_MM
    d2 = x * x + z * z
    if d2 > (l1 + l2 - 0.5) ** 2 or d2 < (l1 - l2) ** 2 + 1.0 or z <= 0:
        return None
    c = (d2 - l1 * l1 - l2 * l2) / (2 * l1 * l2)
    c = max(-1.0, min(1.0, c))
    knee = math.acos(c)
    hip = math.atan2(x, z) + math.atan2(l2 * math.sin(knee),
                                        l1 + l2 * math.cos(knee))
    hip, knee = math.degrees(hip), math.degrees(knee)
    return hip, knee, knee - hip


def leg_points(hip_deg, knee_deg):
    """Hip, knee and foot points for drawing. z is down, so flip for plots."""
    t1 = math.radians(hip_deg)
    t2 = math.radians(hip_deg - knee_deg)
    kx, kz = THIGH_MM * math.sin(t1), THIGH_MM * math.cos(t1)
    fx, fz = kx + SHANK_MM * math.sin(t2), kz + SHANK_MM * math.cos(t2)
    return [(0.0, 0.0), (kx, kz), (fx, fz)]

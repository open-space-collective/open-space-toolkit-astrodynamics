# Apache License 2.0

from ostk.astrodynamics.trajectory import Model


class TestModel:
    def test_out_of_bounds_errors(self):
        assert issubclass(Model.OutOfBoundsError, RuntimeError)
        assert issubclass(Model.BeforeStartError, Model.OutOfBoundsError)
        assert issubclass(Model.AfterEndError, Model.OutOfBoundsError)

        assert not issubclass(Model.BeforeStartError, Model.AfterEndError)
        assert not issubclass(Model.AfterEndError, Model.BeforeStartError)
